#pragma once

#include "drawing/paint_style.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/text_style.hpp"

namespace paint::drawing {

// 畫布目前的樣式設定，新物件建立時會複製需要的部分。
struct StyleSet {
    PaintStyle paint;
    ShapeStyle shape;
    TextStyle text;
};

}  // namespace paint::drawing
