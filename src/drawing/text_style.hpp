#pragma once

#include "common/color.hpp"
#include "common/font.hpp"

namespace paint::drawing {

struct TextStyle {
    FontStyle font = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18};

    ColorRGBA color = Color::Black;
    float lineSpacing = 1.0f;
};

}  // namespace paint::drawing
