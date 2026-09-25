#include "drawing_tool.hpp"

#include <algorithm>
#include <cmath>

namespace drawing {
namespace {

// 直線、矩形、圓形等需要拖曳兩個點的工具可以共用這個基底類別。
template <typename TShape>
class DragTool : public DrawingTool<TShape> {
   public:
    using DrawingTool<TShape>::DrawingTool;

    ToolEventResult onMouseDown(ToolEventState& event) override {
        this->draft->setStart(event.mousePosition);
        this->draft->setEnd(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        this->draft->setEnd(event.mousePosition);
        if (event.specialKeyStates[GLUT_KEY_SHIFT_L] || event.specialKeyStates[GLUT_KEY_SHIFT_R]) {
            onShift(event);
        }
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        this->draft->setEnd(event.mousePosition);
        if (event.specialKeyStates[GLUT_KEY_SHIFT_L] || event.specialKeyStates[GLUT_KEY_SHIFT_R]) {
            onShift(event);
        }
        return ToolEventResult::COMMIT;
    }

    ToolEventResult onKeyDown(ToolEventState& event) override {
        if (event.keyStates[27]) {  // ESC
            return ToolEventResult::CANCEL;
        }
        if (event.specialKeyStates[GLUT_KEY_SHIFT_L] || event.specialKeyStates[GLUT_KEY_SHIFT_R]) {
            onShift(event);
        }
        return ToolEventResult::NONE;
    }

   protected:
    // Shift 鍵被按下時，會畫出正方形、正圓或 45° 斜線。這個函式可以被子類別覆寫以實現不同的行為。
    virtual void onShift(ToolEventState& event) {
        const Point& start = this->draft->getStart();
        const Point& now = event.mousePosition;

        GLfloat dx = now.getX() - start.getX();
        GLfloat dy = now.getY() - start.getY();

        const auto size = std::max(std::abs(dx), std::abs(dy));
        Point newEnd;
        newEnd.setX(start.getX() + (dx >= 0 ? size : -size));
        newEnd.setY(start.getY() + (dy >= 0 ? size : -size));
        this->draft->setEnd(newEnd);
    }
};

class LineTool : public DragTool<shape::Line> {
   public:
    using DragTool<shape::Line>::DragTool;

   protected:
    // Shift 鍵被按下時，會畫出水平、垂直或 45° 斜線。
    void onShift(ToolEventState& event) override {
        const Point& start = this->draft->getStart();
        const Point& now = event.mousePosition;

        const GLfloat dx = now.getX() - start.getX();
        const GLfloat dy = now.getY() - start.getY();
        const GLfloat ax = std::abs(dx);
        const GLfloat ay = std::abs(dy);

        // tan(22.5°)：水平、斜線、垂直之間的分界。
        const GLfloat threshold = static_cast<GLfloat>(std::tan(M_PI / 8.0f));

        Point newEnd = start;
        if (ay <= ax * threshold) {
            // 水平線：固定 Y。
            newEnd.setX(start.getX() + dx);
        } else if (ax <= ay * threshold) {
            // 垂直線：固定 X。
            newEnd.setY(start.getY() + dy);
        } else {
            // 45° 斜線：投影到最近的對角線。
            const auto size = std::max(ax, ay);
            newEnd.setX(start.getX() + (dx >= 0 ? size : -size));
            newEnd.setY(start.getY() + (dy >= 0 ? size : -size));
        }

        this->draft->setEnd(newEnd);
    }
};

class RectangleTool : public DragTool<shape::Rectangle> {
   public:
    using DragTool<shape::Rectangle>::DragTool;
};

class EllipseTool : public DragTool<shape::Ellipse> {
   public:
    using DragTool<shape::Ellipse>::DragTool;
};

class PencilTool : public DrawingTool<shape::Stroke> {
   public:
    using DrawingTool<shape::Stroke>::DrawingTool;

    ToolEventResult onMouseDown(ToolEventState& event) override {
        draft->addPoint(event.mousePosition, true);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseMove(ToolEventState& event) override {
        draft->addPoint(event.mousePosition);
        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(ToolEventState& event) override {
        draft->addPoint(event.mousePosition, true);
        return ToolEventResult::COMMIT;
    }
};

class PolygonTool : public DrawingTool<shape::Polygon> {
   public:
    using DrawingTool<shape::Polygon>::DrawingTool;

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

std::unique_ptr<IDrawingTool> createDrawingTool(canvas::Tool tool, shape::ShapeStyle style) {
    switch (tool) {
    case canvas::Tool::TOOL_PENCIL:
        return std::make_unique<PencilTool>(style);
    case canvas::Tool::TOOL_LINE:
        return std::make_unique<LineTool>(style);
    case canvas::Tool::TOOL_RECTANGLE:
        return std::make_unique<RectangleTool>(style);
    case canvas::Tool::TOOL_ELLIPSE:
        return std::make_unique<EllipseTool>(style);
    case canvas::Tool::TOOL_POLYGON:
        return std::make_unique<PolygonTool>(style);
    default:
        return nullptr;
    }
}

}  // namespace drawing
