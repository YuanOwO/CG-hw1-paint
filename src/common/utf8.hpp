#pragma once

#include <cstddef>
#include <string>

namespace paint::utf8 {

struct DecodeResult {
    char32_t codepoint;
    std::size_t bytesConsumed;
};

// 單一字元轉換，適合 renderer 和游標操作。
DecodeResult decodeOne(const std::string& text, std::size_t offset);

std::string encodeOne(char32_t codepoint);

// 整個字串轉換，適合一般使用。
std::u32string toUtf32(const std::string& text);
std::string fromUtf32(const std::u32string& text);

}  // namespace paint::utf8
