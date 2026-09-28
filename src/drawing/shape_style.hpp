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

    void setColor(const ColorRGBA& c) { color = c; }
    const ColorRGBA& getColor() const { return color; }
};

struct StrokeStyle {
    float width = 1.0f;
    ColorRGBA color;

    LineJoin join = LineJoin::MITER;
    LineCap cap = LineCap::ROUND;

    // 避免非常尖的角產生超長 miter
    float miterLimit = 4.0f;

    // Setters and Getters

    void setWidth(float w) { width = std::max(1.0f, w); }
    float getWidth() const { return width; }

    void setColor(const ColorRGBA& c) { color = c; }
    const ColorRGBA& getColor() const { return color; }

    void setJoin(LineJoin j) { join = j; }
    LineJoin getJoin() const { return join; }

    void setCap(LineCap c) { cap = c; }
    LineCap getCap() const { return cap; }

    void setMiterLimit(float limit) { miterLimit = std::max(1.0f, limit); }
    float getMiterLimit() const { return miterLimit; }
};

struct ShapeStyle {
    FillMode fillMode = FillMode::ADVANCED;
    FillStyle fill;
    StrokeStyle stroke;
    float pointSize = 1.0f;  // 用於繪製點的大小

    // Setters and Getters

    void setFillMode(FillMode mode) { fillMode = mode; }
    const FillMode getFillMode() const { return fillMode; }

    void setPointSize(float size) { pointSize = std::max(1.0f, size); }
    float getPointSize() const { return pointSize; }

    // Fill

    void setFill(const FillStyle& f) { fill = f; }
    const FillStyle& getFill() const { return fill; }

    void setFillColor(const ColorRGBA& c) { fill.setColor(c); }
    const ColorRGBA& getFillColor() const { return fill.getColor(); }

    // Stroke

    void setStroke(const StrokeStyle& s) { stroke = s; }
    const StrokeStyle& getStroke() const { return stroke; }

    void setStrokeColor(const ColorRGBA& c) { stroke.setColor(c); }
    const ColorRGBA& getStrokeColor() const { return stroke.getColor(); }

    void setStrokeWidth(float w) { stroke.setWidth(w); }
    float getStrokeWidth() const { return stroke.getWidth(); }

    void setStrokeJoin(LineJoin j) { stroke.setJoin(j); }
    LineJoin getStrokeJoin() const { return stroke.getJoin(); }

    void setStrokeCap(LineCap c) { stroke.setCap(c); }
    LineCap getStrokeCap() const { return stroke.getCap(); }

    void setStrokeMiterLimit(float limit) { stroke.setMiterLimit(limit); }
    float getStrokeMiterLimit() const { return stroke.getMiterLimit(); }
};

}  // namespace paint::drawing
