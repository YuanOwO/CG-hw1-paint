#pragma once

#include <utility>

#include "drawing/text_style.hpp"
#include "drawing/tools/canvas_tool.hpp"

namespace paint::drawing {

class TextTool : public ICanvasTool {
   public:
    explicit TextTool(TextStyle style) : _style(std::move(style)) {}

    ToolResult onClick(const ClickEvent& event, Point localPosition) override;

   private:
    TextStyle _style;
};

}  // namespace paint::drawing
