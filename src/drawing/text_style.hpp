#pragma once

#include "common/font.hpp"

namespace paint::drawing {

struct TextStyle {
    FontStyle font = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18};

    float lineSpacing = 1.0f;
};

}  // namespace paint::drawing
