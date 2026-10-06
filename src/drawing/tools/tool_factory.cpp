#include "drawing/tools/tool_factory.hpp"

#include "drawing/tools/select_tool.hpp"
#include "drawing/tools/shape_tools.hpp"
#include "drawing/tools/text_tool.hpp"

namespace paint::drawing {

const std::string getToolName(ToolKind tool) {
    switch (tool) {
    case ToolKind::SELECT:
        return "Select";
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

std::unique_ptr<ICanvasTool> createCanvasTool(ToolKind tool, const Scene& scene, const StyleSet& style) {
    switch (tool) {
    case ToolKind::SELECT:
        return std::make_unique<SelectTool>(scene);
    case ToolKind::POINT:
        return std::make_unique<PointTool>(style.paint, style.shape);
    case ToolKind::PENCIL:
        return std::make_unique<PencilTool>(style.paint, style.shape);
    case ToolKind::LINE:
        return std::make_unique<LineTool>(style.paint, style.shape);
    case ToolKind::RECTANGLE:
        return std::make_unique<RectangleTool>(style.paint, style.shape);
    case ToolKind::ELLIPSE:
        return std::make_unique<EllipseTool>(style.paint, style.shape);
    case ToolKind::POLYGON:
        return std::make_unique<PolygonTool>(style.paint, style.shape);
    case ToolKind::TEXT:
        return std::make_unique<TextTool>(style.paint, style.text);
    default:
        return nullptr;
    }
}

}  // namespace paint::drawing
