#include "drawing/tools/text_tool.hpp"

#include "common/point.hpp"

namespace paint::drawing {

ToolResult TextTool::onClick(const ClickEvent& event, Point localPosition) {
    return ToolResult::requestTextInput(localPosition, _paint, _style);
}

}  // namespace paint::drawing
