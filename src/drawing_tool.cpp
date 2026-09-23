#include "drawing_tool.hpp"

namespace drawing {
namespace {

class LineTool : public DrawingTool<shape::Line> {
   public:
    LineTool(GLfloat width, const GLfloat* color, const GLfloat* fillColor)
        : DrawingTool<shape::Line>(width, color, fillColor) {}

    ToolEventResult onMouseDown(ToolEventState& event) override {
        draft->setStart(event.mousePosition);
        draft->setEnd(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        draft->setEnd(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        draft->setEnd(event.mousePosition);
        return ToolEventResult::COMMIT;
    }
};

class RectangleTool : public DrawingTool<shape::Rectangle> {
   public:
    RectangleTool(GLfloat width, const GLfloat* color, const GLfloat* fillColor)
        : DrawingTool<shape::Rectangle>(width, color, fillColor) {}

    ToolEventResult onMouseDown(ToolEventState& event) override {
        draft->setStart(event.mousePosition);
        draft->setEnd(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        draft->setEnd(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        draft->setEnd(event.mousePosition);
        return ToolEventResult::COMMIT;
    }
};

class PencilTool : public DrawingTool<shape::Stroke> {
   public:
    PencilTool(GLfloat width, const GLfloat* color, const GLfloat* fillColor)
        : DrawingTool<shape::Stroke>(width, color, fillColor) {}

    ToolEventResult onMouseDown(ToolEventState& event) override {
        draft->addPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        draft->addPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        draft->addPoint(event.mousePosition);
        return ToolEventResult::COMMIT;
    }
};

class PolygonTool : public DrawingTool<shape::Polygon> {
   public:
    PolygonTool(GLfloat width, const GLfloat* color, const GLfloat* fillColor)
        : DrawingTool<shape::Polygon>(width, color, fillColor) {}

    ToolEventResult onMouseDown(ToolEventState& event) override {
        if (draft->pointCount() == 0) {
            draft->addPoint(event.mousePosition);
        } else {
            draft->setLastPoint(event.mousePosition);
        }
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        draft->addPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        draft->setLastPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMousePassiveMove(ToolEventState& event) override {
        draft->setLastPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onKeyDown(ToolEventState& event) override {
        if (event.keyStates['\r'] && draft->pointCount() >= 3) {
            return ToolEventResult::COMMIT;
        }
        if (event.keyStates[27]) {  // ESC
            return ToolEventResult::CANCEL;
        }
        if (event.keyStates['\b']) {  // Backspace
            if (draft->pointCount() == 0) {
                return ToolEventResult::CANCEL;
            }
            draft->removeLastPoint();
        }
        return ToolEventResult::NONE;
    }
};

}  // namespace

std::unique_ptr<IDrawingTool> createDrawingTool(draw::Tool tool, GLfloat width, const GLfloat* color,
                                                const GLfloat* fillColor) {
    switch (tool) {
    case draw::Tool::TOOL_PENCIL:
        return std::make_unique<PencilTool>(width, color, fillColor);
    case draw::Tool::TOOL_LINE:
        return std::make_unique<LineTool>(width, color, fillColor);
    case draw::Tool::TOOL_RECTANGLE:
        return std::make_unique<RectangleTool>(width, color, fillColor);
    case draw::Tool::TOOL_POLYGON:
        return std::make_unique<PolygonTool>(width, color, fillColor);
    default:
        return nullptr;  // Circle 尚未實作，維持原行為。
    }
}

}  // namespace drawing
