#pragma once

#include <vector>

#include "common/color.hpp"

namespace paint::drawing {

enum class LineJoin { NONE, MITER, BEVEL, ROUND };

enum class LineCap { BUTT, SQUARE, ROUND };

struct FillStyle {
    bool enabled = true;

    ColorRGBA color;
};

struct StrokeStyle {
    bool enabled = true;

    float width = 1.0f;
    ColorRGBA color;

    LineJoin join = LineJoin::MITER;
    LineCap cap = LineCap::ROUND;

    // 避免非常尖的角產生超長 miter
    float miterLimit = 4.0f;
};

struct ShapeStyle {
    FillStyle fill;
    StrokeStyle stroke;
};

}  // namespace paint::drawing
