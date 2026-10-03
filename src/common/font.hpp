#pragma once

#include <string>
#include <variant>

namespace paint {

enum class BitmapFont {
    BITMAP_8_BY_13,
    BITMAP_9_BY_15,
    BITMAP_HELVETICA_10,
    BITMAP_HELVETICA_12,
    BITMAP_HELVETICA_18,
    BITMAP_TIMES_ROMAN_10,
    BITMAP_TIMES_ROMAN_24,
};

enum class StrokeFont {
    STROKE_ROMAN,
    STROKE_MONO_ROMAN,
};

struct BitmapFontStyle {
    BitmapFont font;

    bool operator==(const BitmapFontStyle& other) const { return font == other.font; }
    bool operator!=(const BitmapFontStyle& other) const { return !(*this == other); }
};

struct StrokeFontStyle {
    StrokeFont font;
    float size = 1.0f;  // 默認大小為 1.0

    bool operator==(const StrokeFontStyle& other) const { return font == other.font && size == other.size; }
    bool operator!=(const StrokeFontStyle& other) const { return !(*this == other); }
};

using Font = std::variant<BitmapFont, StrokeFont>;
using FontStyle = std::variant<BitmapFontStyle, StrokeFontStyle>;

void* mapFont(const BitmapFont& font);
void* mapFont(const StrokeFont& font);
inline void* mapFont(const Font& font) {
    return std::visit([](const auto& f) { return mapFont(f); }, font);
}
inline void* mapFont(const FontStyle& font) {
    return std::visit([](const auto& f) { return mapFont(f.font); }, font);
}

float getFontWidth(const BitmapFontStyle& font, const std::string& text);
float getFontWidth(const StrokeFontStyle& font, const std::string& text);
inline float getFontWidth(const FontStyle& font, const std::string& text) {
    return std::visit([text](const auto& f) { return getFontWidth(f, text); }, font);
}

float getFontHeight(const BitmapFontStyle& font, const std::string& text);
float getFontHeight(const StrokeFontStyle& font, const std::string& text);
inline float getFontHeight(const FontStyle& font, const std::string& text) {
    return std::visit([text](const auto& f) { return getFontHeight(f, text); }, font);
}

}  // namespace paint
