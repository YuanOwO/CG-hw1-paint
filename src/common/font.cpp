#include "common/font.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "common/utf8.hpp"

namespace paint {

namespace {

std::unordered_map<GfntFontId, std::unique_ptr<GfntFont>> g_fonts;

}  // namespace

void initializeFonts(const std::filesystem::path& fontDirectory) {
    g_fonts.clear();
    g_fonts[GfntFontId::CUBIC_11] =
        std::make_unique<GfntFont>(GfntFont::load(fontDirectory / "Cubic-11" / "Cubic_11.gfnt"));
    g_fonts[GfntFontId::UNIFONT_16] =
        std::make_unique<GfntFont>(GfntFont::load(fontDirectory / "unifont" / "unifont_16.gfnt"));
}

void* mapFont(const BitmapFont& font) {
    switch (font) {
    case BitmapFont::BITMAP_8_BY_13:
        return GLUT_BITMAP_8_BY_13;
    case BitmapFont::BITMAP_9_BY_15:
        return GLUT_BITMAP_9_BY_15;
    case BitmapFont::BITMAP_HELVETICA_10:
        return GLUT_BITMAP_HELVETICA_10;
    case BitmapFont::BITMAP_HELVETICA_12:
        return GLUT_BITMAP_HELVETICA_12;
    case BitmapFont::BITMAP_HELVETICA_18:
        return GLUT_BITMAP_HELVETICA_18;
    case BitmapFont::BITMAP_TIMES_ROMAN_10:
        return GLUT_BITMAP_TIMES_ROMAN_10;
    case BitmapFont::BITMAP_TIMES_ROMAN_24:
        return GLUT_BITMAP_TIMES_ROMAN_24;

    default:
        throw std::invalid_argument("Unknown bitmap font");
    }
}

void* mapFont(const StrokeFont& font) {
    switch (font) {
    case StrokeFont::STROKE_ROMAN:
        return GLUT_STROKE_ROMAN;
    case StrokeFont::STROKE_MONO_ROMAN:
        return GLUT_STROKE_MONO_ROMAN;

    default:
        throw std::invalid_argument("Unknown stroke font");
    }
}

const GfntFont& mapFont(const GfntFontId& font) {
    switch (font) {
    case GfntFontId::CUBIC_11:
        if (!g_fonts.count(GfntFontId::CUBIC_11)) {
            throw std::logic_error("Fonts have not been initialized");
        }
        return *g_fonts[GfntFontId::CUBIC_11];
    case GfntFontId::UNIFONT_16:
        if (!g_fonts.count(GfntFontId::UNIFONT_16)) {
            throw std::logic_error("Fonts have not been initialized");
        }
        return *g_fonts[GfntFontId::UNIFONT_16];

    default:
        throw std::invalid_argument("Unknown GFNT font");
    }
}

float getFontWidth(const BitmapFontStyle& font, const std::string& text) {
    void* glutFont = mapFont(font.font);

    int width = 0, maxWidth = 0;
    for (auto c : text) {
        if (c == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            continue;
        }
        width += glutBitmapWidth(glutFont, c);
    }

    return std::max(maxWidth, width);  // 返回最大寬度
}

float getFontWidth(const StrokeFontStyle& font, const std::string& text) {
    void* glutFont = mapFont(font.font);
    if (!glutFont) {
        throw std::invalid_argument("Unknown stroke font");
    }

    int width = 0, maxWidth = 0;
    for (auto c : text) {
        if (c == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            continue;
        }
        width += glutStrokeWidth(glutFont, c);
    }

    return std::max(maxWidth, width) * font.size;  // 返回最大寬度
}

float getFontWidth(const GfntFontStyle& style, const std::string& text) {
    const GfntFont& font = mapFont(style.font);

    int width = 0, maxWidth = 0;
    std::size_t offset = 0;

    while (offset < text.size()) {
        const auto decoded = utf8::decodeOne(text, offset);

        if (decoded.bytesConsumed == 0) {
            break;
        }

        offset += decoded.bytesConsumed;

        if (decoded.codepoint == U'\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0;
            continue;
        }

        const gfnt::GfntGlyphV1* glyph = font.findGlyphOrFallback(decoded.codepoint);

        if (glyph != nullptr) {
            width += glyph->advanceX;
        } else {
            // 字型連 fallback glyph 都沒有時，仍保留一個
            // nominal pixel size，避免後面的字往前重疊。
            width += font.header().pixelSize;
        }
    }

    return std::max(maxWidth, width);
}

float getFontHeight(const BitmapFontStyle& font, const std::string& text) {
    void* glutFont = mapFont(font.font);

    if (text.empty()) {
        return 0;
    }

    auto lines = std::count(text.begin(), text.end(), '\n') + 1;

    if (text.back() == '\n') {
        lines -= 1;  // 如果最後一行是空行，則不計算高度
    }

    return lines * glutBitmapHeight(glutFont);
}

float getFontHeight(const StrokeFontStyle& font, const std::string& text) {
    void* glutFont = mapFont(font.font);
    if (!glutFont) {
        throw std::invalid_argument("Unknown stroke font");
    }

    if (text.empty()) {
        return 0;
    }

    auto lines = std::count(text.begin(), text.end(), '\n') + 1;

    if (text.back() == '\n') {
        lines -= 1;  // 如果最後一行是空行，則不計算高度
    }

    return lines * glutStrokeHeight(glutFont) * font.size;
}

float getFontHeight(const GfntFontStyle& style, const std::string& text) {
    if (text.empty()) {
        return 0.0f;
    }

    const GfntFont& font = mapFont(style.font);

    std::size_t lines = static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n')) + 1;

    // 配合現有 FreeGLUT 字型的行為：
    // 結尾換行不產生額外的空白行高度。
    if (text.back() == '\n') {
        --lines;
    }

    const auto& header = font.header();

    const int lineHeight = header.ascender + header.descender;

    return static_cast<float>(lines) * lineHeight;
}

}  // namespace paint
