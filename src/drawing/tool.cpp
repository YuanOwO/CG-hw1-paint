#include "drawing/tool.hpp"

#include <algorithm>
#include <cmath>

namespace paint::drawing {

namespace {

// 直線、矩形、圓形等需要拖曳兩個點的工具可以共用這個基底類別。
template <typename TShape>
class DragTool : public DrawingTool<TShape> {
   public:
    using DrawingTool<TShape>::DrawingTool;

    ToolEventResult onKeyDown(const KeyboardEvent& event) override {
        if (event.getKey() == Key::Escape) {  // ESC
            return ToolEventResult::CANCEL;
        }

        if (isShiftKey(event.getKey())) {
            onShift(event);
        }

        return ToolEventResult::NONE;
    }

    ToolEventResult onKeyUp(const KeyboardEvent& event) override {
        if (isShiftKey(event.getKey())) {
            this->draft->setEnd(event.getPosition());
        }

        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseDown(const MouseEvent& event) override {
        this->draft->setStart(event.getPosition());
        this->draft->setEnd(event.getPosition());

        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(const MouseEvent& event) override {
        this->draft->setEnd(event.getPosition());
        if (event.getKeyboardState().isShiftDown()) {
            onShift(event);
        }

        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event) override {
        if (!event.getMouseState().isDown(MouseButton::MouseLeft)) {
            return ToolEventResult::NONE;
        }

        this->draft->setEnd(event.getPosition());
        if (event.getKeyboardState().isShiftDown()) {
            onShift(event);
        }

        return ToolEventResult::NONE;
    }

   protected:
    // Shift 鍵被按下時，會畫出正方形、正圓或 45° 斜線。這個函式可以被子類別覆寫以實現不同的行為。
    virtual void onShift(const InputEvent& event) {
        const Point& start = this->draft->getStart();
        const Point& now = event.getPosition();

        float dx = now.getX() - start.getX();
        float dy = now.getY() - start.getY();

        const auto size = std::max(std::abs(dx), std::abs(dy));
        Point newEnd;
        newEnd.setX(start.getX() + (dx >= 0 ? size : -size));
        newEnd.setY(start.getY() + (dy >= 0 ? size : -size));
        this->draft->setEnd(newEnd);
    }
};

class LineTool : public DragTool<Line> {
   public:
    using DragTool<Line>::DragTool;

   protected:
    // Shift 鍵被按下時，會畫出水平、垂直或 45° 斜線。
    void onShift(const InputEvent& event) override {
        const Point& start = this->draft->getStart();
        const Point& now = event.getPosition();

        const float dx = now.getX() - start.getX();
        const float dy = now.getY() - start.getY();
        const float ax = std::abs(dx);
        const float ay = std::abs(dy);

        // tan(22.5°)：水平、斜線、垂直之間的分界。
        const float threshold = static_cast<float>(std::tan(M_PI / 8.0f));

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

class RectangleTool : public DragTool<Rectangle> {
   public:
    using DragTool<Rectangle>::DragTool;
};

class EllipseTool : public DragTool<Ellipse> {
   public:
    using DragTool<Ellipse>::DragTool;
};

class PointTool : public DrawingTool<PointShape> {
   public:
    using DrawingTool<PointShape>::DrawingTool;

    ToolEventResult onClick(const MouseClickEvent& event) override {
        draft->setPosition(event.getPosition());
        return ToolEventResult::COMMIT;
    }
};

class PencilTool : public DrawingTool<Path> {
   public:
    using DrawingTool<Path>::DrawingTool;

    ToolEventResult onMouseDown(const MouseEvent& event) override {
        draft->addPoint(event.getPosition(), true);

        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseUp(const MouseEvent& event) override {
        draft->addPoint(event.getPosition(), true);

        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event) override {
        if (!event.getMouseState().isDown(MouseButton::MouseLeft)) {
            return ToolEventResult::NONE;
        }

        draft->addPoint(event.getPosition());

        return ToolEventResult::NONE;
    }
};

class PolygonTool : public DrawingTool<Polygon> {
   public:
    using DrawingTool<Polygon>::DrawingTool;

    ToolEventResult onClick(const MouseClickEvent& event) override {
        // 第一次點擊時，加入第一個點；之後的點擊，更新最後一個點並加入新點。
        if (draft->pointCount() == 0) {
            draft->addPoint(event.getPosition());
        } else {
            draft->setLastPoint(event.getPosition());
        }
        draft->addPoint(event.getPosition());

        return ToolEventResult::NONE;
    }

    ToolEventResult onDoubleClick(const MouseClickEvent& event) override {
        draft->setLastPoint(event.getPosition());
        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event) override {
        if (event.getMouseState().isUp(MouseButton::MouseLeft) && draft->pointCount() == 0) {
            // 尚未加入任何點，無法預覽。
            return ToolEventResult::CANCEL;
        }

        draft->setLastPoint(event.getPosition());
        return ToolEventResult::NONE;
    }

    ToolEventResult onKeyDown(const KeyboardEvent& event) override {
        if (event.getKey() == Key::Enter) {
            return ToolEventResult::COMMIT;
        }

        if (event.getKey() == Key::Escape) {
            return ToolEventResult::CANCEL;
        }

        if (event.getKey() == Key::Backspace) {
            if (draft->pointCount() == 0) {
                return ToolEventResult::CANCEL;
            }
            draft->removeLastPoint();
        }

        return ToolEventResult::NONE;
    }
};

}  // namespace

std::unique_ptr<IDrawingTool> createDrawingTool(Tool tool, ShapeStyle style) {
    switch (tool) {
    case Tool::TOOL_POINT:
        return std::make_unique<PointTool>(style);
    case Tool::TOOL_PENCIL:
        return std::make_unique<PencilTool>(style);
    case Tool::TOOL_LINE:
        return std::make_unique<LineTool>(style);
    case Tool::TOOL_RECTANGLE:
        return std::make_unique<RectangleTool>(style);
    case Tool::TOOL_ELLIPSE:
        return std::make_unique<EllipseTool>(style);
    case Tool::TOOL_POLYGON:
        return std::make_unique<PolygonTool>(style);
    default:
        return nullptr;
    }
}

}  // namespace paint::drawing
