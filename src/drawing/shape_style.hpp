#pragma once

#include <algorithm>
#include <vector>

#include "common/color.hpp"

namespace paint::drawing {

enum class FillMode { OUTLINE, FILLED, ADVANCED };

enum class LineJoin { NONE, MITER, BEVEL, ROUND };

enum class LineCap { BUTT, SQUARE, ROUND };

struct FillStyle {
    ColorRGBA color;

    // Setters and Getters

    // const ColorRGBA& color() const { return color; }
    void setColor(const ColorRGBA& c) { color = c; }
};

struct StrokeStyle {
    float width = 1.0f;
    ColorRGBA color;

    LineJoin join = LineJoin::MITER;
    LineCap cap = LineCap::ROUND;

    // 避免非常尖的角產生超長 miter
    float miterLimit = 4.0f;

    // Setters and Getters

    // float width() const { return width; }
    void setWidth(float w) { width = std::max(1.0f, w); }

    // const ColorRGBA& color() const { return color; }
    void setColor(const ColorRGBA& c) { color = c; }

    // LineJoin join() const { return join; }
    void setJoin(LineJoin j) { join = j; }

    // LineCap cap() const { return cap; }
    void setCap(LineCap c) { cap = c; }

    // float miterLimit() const { return miterLimit; }
    void setMiterLimit(float limit) { miterLimit = std::max(1.0f, limit); }
};

struct ShapeStyle {
    FillMode fillMode = FillMode::ADVANCED;
    FillStyle fill;
    StrokeStyle stroke;
    float pointSize = 1.0f;  // 用於繪製點的大小

    // Setters and Getters

    // const FillMode fillMode() const { return fillMode; }
    void setFillMode(FillMode mode) { fillMode = mode; }

    // float pointSize() const { return pointSize; }
    void setPointSize(float size) { pointSize = std::max(1.0f, size); }

    // Fill

    const FillStyle& fillStyle() const { return fill; }
    void setFillStyle(const FillStyle& f) { fill = f; }

    const ColorRGBA& fillColor() const { return fill.color; }
    void setFillColor(const ColorRGBA& c) { fill.setColor(c); }

    // Stroke

    const StrokeStyle& strokeStyle() const { return stroke; }
    void setStrokeStyle(const StrokeStyle& s) { stroke = s; }

    const ColorRGBA& strokeColor() const { return stroke.color; }
    void setStrokeColor(const ColorRGBA& c) { stroke.setColor(c); }

    float strokeWidth() const { return stroke.width; }
    void setStrokeWidth(float w) { stroke.setWidth(w); }

    LineJoin strokeJoin() const { return stroke.join; }
    void setStrokeJoin(LineJoin j) { stroke.setJoin(j); }

    LineCap strokeCap() const { return stroke.cap; }
    void setStrokeCap(LineCap c) { stroke.setCap(c); }

    float strokeMiterLimit() const { return stroke.miterLimit; }
    void setStrokeMiterLimit(float limit) { stroke.setMiterLimit(limit); }
};

}  // namespace paint::drawing
