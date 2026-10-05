#include "drawing/tools/text_tool.hpp"

namespace paint::drawing {

ToolResult TextTool::onClick(const ClickEvent& event, Point localPosition) {
    return ToolResult::requestTextInput(localPosition, _style);
}

}  // namespace paint::drawing
