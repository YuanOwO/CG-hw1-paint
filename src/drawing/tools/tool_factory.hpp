#pragma once

#include <memory>
#include <string>

#include "drawing/shape_style.hpp"
#include "drawing/text_style.hpp"
#include "drawing/tools/canvas_tool.hpp"

namespace paint::drawing {

class Scene;

enum class ToolKind {
    SELECT,
    POINT,
    PENCIL,
    LINE,
    RECTANGLE,
    ELLIPSE,
    POLYGON,
    TEXT,
};

const std::string getToolName(ToolKind tool);

std::unique_ptr<ICanvasTool> createCanvasTool(ToolKind tool, const Scene& scene, ShapeStyle shapeStyle,
                                              TextStyle textStyle);

}  // namespace paint::drawing
