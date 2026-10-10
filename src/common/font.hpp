#pragma once

#include <filesystem>
#include <string>
#include <variant>

#include "common/gfnt_font.hpp"

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

enum class GfntFontId {
    CUBIC_11,
    UNIFONT_16,
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

struct GfntFontStyle {
    GfntFontId font;

    bool operator==(const GfntFontStyle& other) const { return font == other.font; }
    bool operator!=(const GfntFontStyle& other) const { return !(*this == other); }
};

using Font = std::variant<BitmapFont, StrokeFont>;
using FontStyle = std::variant<BitmapFontStyle, StrokeFontStyle, GfntFontStyle>;

void* mapFont(const BitmapFont& font);
void* mapFont(const StrokeFont& font);
const GfntFont& mapFont(const GfntFontId& font);

// 內建 GFNT 只會載入一次，FontStyle 本身只需要保存 enum。
void initializeFonts(const std::filesystem::path& fontDirectory);

inline void* mapFont(const Font& font) {
    return std::visit([](const auto& f) { return mapFont(f); }, font);
}

// GLUT 內建字型的度量，數值與 FreeGLUT 的字型資料相同。
// Apple GLUT 沒有 glutBitmapHeight / glutStrokeHeight，因此自行查表。

// Bitmap 字型的行高（像素）
float getLineHeight(const BitmapFont& font);

// Bitmap 字型的 raster origin Y。基線位置 = 行頂端 + 行高 - originY。
float getOriginY(const BitmapFont& font);

// Stroke 字型的行高與上緣，單位為字型座標，需乘上 StrokeFontStyle::size
float getLineHeight(const StrokeFont& font);
float getAscent(const StrokeFont& font);

float getFontWidth(const BitmapFontStyle& font, const std::string& text);
float getFontWidth(const StrokeFontStyle& font, const std::string& text);
float getFontWidth(const GfntFontStyle& font, const std::string& text);

inline float getFontWidth(const FontStyle& font, const std::string& text) {
    return std::visit([&text](const auto& f) { return getFontWidth(f, text); }, font);
}

float getFontHeight(const BitmapFontStyle& font, const std::string& text);
float getFontHeight(const StrokeFontStyle& font, const std::string& text);
float getFontHeight(const GfntFontStyle& font, const std::string& text);

inline float getFontHeight(const FontStyle& font, const std::string& text) {
    return std::visit([&text](const auto& f) { return getFontHeight(f, text); }, font);
}

}  // namespace paint
