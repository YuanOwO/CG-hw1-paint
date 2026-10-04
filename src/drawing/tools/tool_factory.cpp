#include "drawing/tools/tool_factory.hpp"

#include <utility>

#include "drawing/tools/shape_tools.hpp"
#include "drawing/tools/text_tool.hpp"

namespace paint::drawing {

const std::string getToolName(ToolKind tool) {
    switch (tool) {
    case ToolKind::POINT:
        return "Point";
    case ToolKind::PENCIL:
        return "Pencil";
    case ToolKind::LINE:
        return "Line";
    case ToolKind::RECTANGLE:
        return "Rectangle";
    case ToolKind::ELLIPSE:
        return "Ellipse";
    case ToolKind::POLYGON:
        return "Polygon";
    case ToolKind::TEXT:
        return "Text";
    default:
        return "Unknown";
    }
}

std::unique_ptr<ICanvasTool> createCanvasTool(ToolKind tool, ShapeStyle shapeStyle, TextStyle textStyle) {
    switch (tool) {
    case ToolKind::POINT:
        return std::make_unique<PointTool>(shapeStyle);
    case ToolKind::PENCIL:
        return std::make_unique<PencilTool>(shapeStyle);
    case ToolKind::LINE:
        return std::make_unique<LineTool>(shapeStyle);
    case ToolKind::RECTANGLE:
        return std::make_unique<RectangleTool>(shapeStyle);
    case ToolKind::ELLIPSE:
        return std::make_unique<EllipseTool>(shapeStyle);
    case ToolKind::POLYGON:
        return std::make_unique<PolygonTool>(shapeStyle);
    case ToolKind::TEXT:
        return std::make_unique<TextTool>(std::move(textStyle));
    default:
        return nullptr;
    }
}

}  // namespace paint::drawing
