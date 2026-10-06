#pragma once

#include <string>

#include "common/color.hpp"
#include "common/font.hpp"
#include "common/point.hpp"
#include "drawing/paint_style.hpp"
#include "drawing/scene_object.hpp"
#include "drawing/text_style.hpp"

namespace paint::drawing {

class TextObject : public SceneObject {
   public:
    TextObject(const Point& position, const std::string& text, const PaintStyle& paint,
               const TextStyle& style)
        : _position(position), _text(text), _paint(paint), _style(style) {}

    ObjectKind objectKind() const override { return ObjectKind::Text; }

    const Point& position() const { return _position; }
    void setPosition(Point position) { _position = position; }

    const std::string& text() const { return _text; }

    const PaintStyle& paint() const { return _paint; }
    const TextStyle& style() const { return _style; }

   private:
    Point _position;
    std::string _text;
    PaintStyle _paint;
    TextStyle _style;
};

}  // namespace paint::drawing
