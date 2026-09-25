#pragma once

#include <GL/freeglut.h>

#include <vector>

#include "color.hpp"

namespace shape {

enum class LineJoin { NONE, MITER, BEVEL, ROUND };

enum class LineCap { BUTT, SQUARE, ROUND };

struct FillStyle {
    bool enabled = true;

    color::ColorRGBA color;
};

struct StrokeStyle {
    bool enabled = true;

    GLfloat width = 1.0f;
    color::ColorRGBA color;

    LineJoin join = LineJoin::MITER;
    LineCap cap = LineCap::ROUND;

    // 避免非常尖的角產生超長 miter
    GLfloat miterLimit = 4.0f;
};

struct ShapeStyle {
    FillStyle fill;
    StrokeStyle stroke;
};

}  // namespace shape
