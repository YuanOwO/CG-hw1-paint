#include "common/font.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <stdexcept>

namespace paint {

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

}  // namespace paint
