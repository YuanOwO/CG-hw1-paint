#pragma once

#include <algorithm>

namespace paint::drawing {

enum class FillMode { OUTLINE, FILLED, ADVANCED };

enum class LineJoin { NONE, MITER, BEVEL, ROUND };

enum class LineCap { BUTT, SQUARE, ROUND };

struct StrokeStyle {
    float width = 1.0f;

    LineJoin join = LineJoin::ROUND;
    LineCap cap = LineCap::ROUND;

    // 避免非常尖的角產生超長 miter
    float miterLimit = 4.0f;

    void setWidth(float w) { width = std::max(1.0f, w); }
    void setMiterLimit(float limit) { miterLimit = std::max(1.0f, limit); }
};

struct ShapeStyle {
    FillMode fillMode = FillMode::ADVANCED;
    StrokeStyle stroke;
    float pointSize = 1.0f;  // 用於繪製點的大小

    void setPointSize(float size) { pointSize = std::max(1.0f, size); }
};

}  // namespace paint::drawing
