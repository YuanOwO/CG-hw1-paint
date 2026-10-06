#pragma once

#include <utility>

#include "drawing/paint_style.hpp"
#include "drawing/text_style.hpp"
#include "drawing/tools/canvas_tool.hpp"

namespace paint::drawing {

class TextTool : public ICanvasTool {
   public:
    TextTool(const PaintStyle& paint, TextStyle style) : _paint(paint), _style(std::move(style)) {}

    ToolResult onClick(const ClickEvent& event, Point localPosition) override;

   private:
    PaintStyle _paint;
    TextStyle _style;
};

}  // namespace paint::drawing
