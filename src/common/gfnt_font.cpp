#include "common/gfnt_font.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace paint {

namespace {

// 所有欄位都用這個 helper 讀，這樣遇到截斷的檔案時，
// 不會把只讀到一半的資料當成有效字型繼續處理。
void readExact(std::istream& input, void* destination, std::size_t size) {
    input.read(static_cast<char*>(destination), static_cast<std::streamsize>(size));

    if (!input) {
        throw std::runtime_error("Unexpected end of GFNT file");
    }
}

// GFNT 在檔案中固定使用 little-endian。這裡手動組合 bytes，
// 避免 loader 暗中依賴執行平台的 endian。
std::uint16_t readU16(std::istream& input) {
    std::array<std::uint8_t, 2> bytes{};
    readExact(input, bytes.data(), bytes.size());

    return static_cast<std::uint16_t>(bytes[0]) | (static_cast<std::uint16_t>(bytes[1]) << 8);
}

std::int16_t readI16(std::istream& input) {
    const std::uint16_t value = readU16(input);

    if (value <= static_cast<std::uint16_t>(std::numeric_limits<std::int16_t>::max())) {
        return static_cast<std::int16_t>(value);
    }

    // 用明確的運算還原 two's-complement，不依賴 unsigned 轉 signed
    // 超出範圍時的 implementation-defined 行為。
    return static_cast<std::int16_t>(static_cast<std::int32_t>(value) - 0x10000);
}

std::uint32_t readU32(std::istream& input) {
    std::array<std::uint8_t, 4> bytes{};
    readExact(input, bytes.data(), bytes.size());

    return static_cast<std::uint32_t>(bytes[0]) | (static_cast<std::uint32_t>(bytes[1]) << 8) |
           (static_cast<std::uint32_t>(bytes[2]) << 16) | (static_cast<std::uint32_t>(bytes[3]) << 24);
}

gfnt::GfntHeaderV1 readHeader(std::istream& input) {
    gfnt::GfntHeaderV1 header{};

    readExact(input, header.magic, sizeof(header.magic));
    header.version = readU16(input);
    header.pixelSize = readU16(input);
    header.ascender = readI16(input);
    header.descender = readI16(input);
    header.glyphCount = readU32(input);

    return header;
}

gfnt::GfntGlyphV1 readGlyph(std::istream& input) {
    gfnt::GfntGlyphV1 glyph{};

    glyph.codepoint = readU32(input);
    glyph.width = readU16(input);
    glyph.height = readU16(input);
    glyph.bearingX = readI16(input);
    glyph.bearingY = readI16(input);
    glyph.advanceX = readU16(input);
    glyph.bitmapOffset = readU32(input);

    return glyph;
}

}  // namespace

GfntFont GfntFont::load(const std::filesystem::path& path) {
    // 先取得檔案大小，之後才能在分配 glyph table 前確認
    // header 裡的 glyphCount 沒有指向檔案外。
    std::ifstream input(path, std::ios::binary | std::ios::ate);

    if (!input) {
        throw std::runtime_error("Cannot open GFNT font: " + path.string());
    }

    const std::streamoff fileSize = input.tellg();

    if (fileSize < static_cast<std::streamoff>(sizeof(gfnt::GfntHeaderV1))) {
        throw std::runtime_error("GFNT file is too small");
    }

    input.seekg(0);

    GfntFont font;
    font._header = readHeader(input);

    if (std::memcmp(font._header.magic, gfnt::MAGIC, sizeof(gfnt::MAGIC)) != 0) {
        throw std::runtime_error("Invalid GFNT magic");
    }

    if (font._header.version != gfnt::VERSION) {
        throw std::runtime_error("Unsupported GFNT version: " + std::to_string(font._header.version));
    }

    if (font._header.pixelSize == 0) {
        throw std::runtime_error("GFNT pixelSize must not be zero");
    }

    if (font._header.ascender < 0 || font._header.descender < 0) {
        throw std::runtime_error("GFNT ascender and descender must be non-negative");
    }

    const std::uint64_t tableSize =
        static_cast<std::uint64_t>(font._header.glyphCount) * sizeof(gfnt::GfntGlyphV1);

    // Bitmap Data 緊接在 Glyph Table 後面，所以不需要額外的
    // section offset 欄位。
    const std::uint64_t bitmapDataOffset = sizeof(gfnt::GfntHeaderV1) + tableSize;

    if (bitmapDataOffset > static_cast<std::uint64_t>(fileSize)) {
        throw std::runtime_error("GFNT glyph table extends past end of file");
    }

    font._glyphs.reserve(font._header.glyphCount);

    for (std::uint32_t i = 0; i < font._header.glyphCount; i++) {
        font._glyphs.push_back(readGlyph(input));
    }

    const std::size_t bitmapDataSize =
        static_cast<std::size_t>(static_cast<std::uint64_t>(fileSize) - bitmapDataOffset);

    font._bitmapData.resize(bitmapDataSize);

    if (bitmapDataSize != 0) {
        readExact(input, font._bitmapData.data(), font._bitmapData.size());
    }

    // 載入時一次驗證完整個字型，渲染時就可以直接取用 bitmap，
    // 不必每畫一個字都重複做邊界檢查。
    for (std::size_t i = 0; i < font._glyphs.size(); i++) {
        const auto& glyph = font._glyphs[i];

        // findGlyph() 使用 binary search，因此這裡不只要檢查排序，
        // 也要拒絕重複的 codepoint。
        if (i != 0 && font._glyphs[i - 1].codepoint >= glyph.codepoint) {
            throw std::runtime_error("GFNT glyph table is not strictly sorted");
        }

        // Unicode code point 的最大合法值是 U+10FFFF，
        // U+D800 至 U+DFFF 為 UTF-16 surrogate，不是合法字元。
        if (glyph.codepoint > 0x10FFFF || (glyph.codepoint >= 0xD800 && glyph.codepoint <= 0xDFFF)) {
            throw std::runtime_error("GFNT contains an invalid Unicode code point");
        }

        const std::size_t bitmapOffset = glyph.bitmapOffset;
        const std::size_t bitmapSize =
            static_cast<std::size_t>(glyph.width) * static_cast<std::size_t>(glyph.height);

        // 用減法比較可以避免 bitmapOffset + bitmapSize 溢位。
        if (bitmapOffset > font._bitmapData.size() || bitmapSize > font._bitmapData.size() - bitmapOffset) {
            throw std::runtime_error("GFNT glyph bitmap extends past Bitmap Data");
        }
    }

    return font;
}

const gfnt::GfntGlyphV1* GfntFont::findGlyph(char32_t codepoint) const {
    const auto value = static_cast<std::uint32_t>(codepoint);

    // Glyph Table 在載入時已確認為 codepoint 遞增排列。
    const auto it = std::lower_bound(_glyphs.begin(), _glyphs.end(), value,
                                     [](const gfnt::GfntGlyphV1& glyph, std::uint32_t searchedCodepoint) {
                                         return glyph.codepoint < searchedCodepoint;
                                     });

    if (it == _glyphs.end() || it->codepoint != value) {
        return nullptr;
    }

    return &*it;
}

const gfnt::GfntGlyphV1* GfntFont::findGlyphOrFallback(char32_t codepoint) const {
    if (const auto* glyph = findGlyph(codepoint)) {
        return glyph;
    }

    if (const auto* glyph = findGlyph(U'\uFFFD')) {
        return glyph;
    }

    return findGlyph(U'?');
}

}  // namespace paint
