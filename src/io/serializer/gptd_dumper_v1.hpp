#pragma once

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "app/document.hpp"
#include "common/color.hpp"
#include "common/font.hpp"
#include "common/point.hpp"
#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"

namespace paint::io {

class GptdDumperV1 {
   public:
    GptdDumperV1(std::ostream& output) : _output(output) {}

    void dump(const app::DocumentData& document);

   private:
    std::ostream& _output;

    std::runtime_error makeSerializationError(const std::string& message);

    void writeColor(const ColorRGBA& color);
    void writePoint(const Point& point);

    const char* fillModeName(drawing::FillMode mode);
    const char* lineJoinName(drawing::LineJoin join);
    const char* lineCapName(drawing::LineCap cap);
    const char* shapeKindName(drawing::ShapeKind kind);

    void writeShapeStyle(const drawing::PaintStyle& paint, const drawing::ShapeStyle& style);
    void writeTwoPointGeometry(const drawing::TwoPointShape& shape);
    void writePointList(const std::vector<Point>& points);
    void writeShape(const drawing::ShapeObject& shape);

    const char* bitmapFontName(BitmapFont font);
    const char* strokeFontName(StrokeFont font);
    const char* gfntFontName(GfntFontId font);
    void writeFont(const FontStyle& font);

    void writeText(const drawing::TextObject& text);
    void writeObject(const drawing::SceneObject& object);
};

}  // namespace paint::io
