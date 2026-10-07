#include "common/utf8.hpp"

namespace paint::utf8 {

namespace {

constexpr char32_t REPLACEMENT_CHARACTER = U'\uFFFD';

bool isSurrogate(char32_t codepoint) {
    return codepoint >= 0xD800 && codepoint <= 0xDFFF;
}

}  // namespace

DecodeResult decodeOne(const std::string& text, std::size_t offset) {
    if (offset >= text.size()) {
        return {U'\0', 0};
    }

    const auto first = static_cast<unsigned char>(text[offset]);

    // ASCII 的最高 bit 為 0，可以直接轉成 code point。
    if (first <= 0x7F) {
        return {static_cast<char32_t>(first), 1};
    }

    std::size_t length;
    char32_t codepoint;
    char32_t minimumValue;

    if (first >= 0xC2 && first <= 0xDF) {
        length = 2;
        codepoint = first & 0x1F;
        minimumValue = 0x80;
    } else if (first >= 0xE0 && first <= 0xEF) {
        length = 3;
        codepoint = first & 0x0F;
        minimumValue = 0x800;
    } else if (first >= 0xF0 && first <= 0xF4) {
        length = 4;
        codepoint = first & 0x07;
        minimumValue = 0x10000;
    } else {
        // Continuation byte、C0、C1，以及 F5 以上都不能作為
        // 合法 UTF-8 sequence 的開頭。
        return {REPLACEMENT_CHARACTER, 1};
    }

    // Sequence 在字串結尾被截斷。
    if (length > text.size() - offset) {
        return {REPLACEMENT_CHARACTER, 1};
    }

    for (std::size_t i = 1; i < length; i++) {
        const auto byte = static_cast<unsigned char>(text[offset + i]);

        // Continuation byte 必須符合 10xxxxxx。
        if ((byte & 0xC0) != 0x80) {
            return {REPLACEMENT_CHARACTER, 1};
        }

        codepoint = (codepoint << 6) | static_cast<char32_t>(byte & 0x3F);
    }

    // 排除 overlong encoding、不存在的 code point，
    // 以及保留給 UTF-16 使用的 surrogate 範圍。
    if (codepoint < minimumValue || codepoint > 0x10FFFF || isSurrogate(codepoint)) {
        return {REPLACEMENT_CHARACTER, 1};
    }

    return {codepoint, length};
}

std::string encodeOne(char32_t codepoint) {
    if (codepoint > 0x10FFFF || isSurrogate(codepoint)) {
        codepoint = REPLACEMENT_CHARACTER;
    }

    std::string result;
    result.reserve(4);

    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    return result;
}

std::u32string toUtf32(const std::string& text) {
    std::u32string result;
    std::size_t offset = 0;

    while (offset < text.size()) {
        const auto decoded = decodeOne(text, offset);

        if (decoded.bytesConsumed == 0) {
            break;
        }

        result.push_back(decoded.codepoint);
        offset += decoded.bytesConsumed;
    }

    return result;
}

std::string fromUtf32(const std::u32string& text) {
    std::string result;

    for (char32_t codepoint : text) {
        result += encodeOne(codepoint);
    }

    return result;
}

}  // namespace paint::utf8
