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

    ToolEventResult onKeyDown(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (event.key() == Key::Escape) {  // ESC
            return ToolEventResult::CANCEL;
        }

        if (isShiftKey(event.key())) {
            onShift(event, localPosition);
            return ToolEventResult::UPDATE;
        }

        return ToolEventResult::NONE;
    }

    ToolEventResult onKeyUp(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (isShiftKey(event.key())) {
            this->_draft->setEnd(localPosition);
            return ToolEventResult::UPDATE;
        }

        return ToolEventResult::NONE;
    }

    ToolEventResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override {
        this->beginDraft();
        this->_draft->setStart(localPosition);
        this->_draft->setEnd(localPosition);

        return ToolEventResult::UPDATE;
    }

    ToolEventResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        this->_draft->setEnd(localPosition);
        if (event.keyboardState().isShiftDown()) {
            onShift(event, localPosition);
        }

        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (!event.mouseState().isDown(MouseButton::MouseLeft)) {
            return ToolEventResult::NONE;
        }

        this->_draft->setEnd(localPosition);
        if (event.keyboardState().isShiftDown()) {
            onShift(event, localPosition);
        }

        return ToolEventResult::UPDATE;
    }

   protected:
    // Shift 鍵被按下時，會畫出正方形、正圓或 45° 斜線。這個函式可以被子類別覆寫以實現不同的行為。
    virtual void onShift(const InputEvent& event, Point localPosition) {
        const Point& start = this->_draft->start();
        const Point& now = localPosition;

        float dx = now.x() - start.x();
        float dy = now.y() - start.y();

        const auto size = std::max(std::abs(dx), std::abs(dy));
        Point newEnd;
        newEnd.setX(start.x() + (dx >= 0 ? size : -size));
        newEnd.setY(start.y() + (dy >= 0 ? size : -size));
        this->_draft->setEnd(newEnd);
    }
};

class LineTool : public DragTool<Line> {
   public:
    using DragTool<Line>::DragTool;

   protected:
    // Shift 鍵被按下時，會畫出水平、垂直或 45° 斜線。
    void onShift(const InputEvent& event, Point localPosition) override {
        const Point& start = this->_draft->start();
        const Point& now = localPosition;

        const float dx = now.x() - start.x();
        const float dy = now.y() - start.y();
        const float ax = std::abs(dx);
        const float ay = std::abs(dy);

        // tan(22.5°)：水平、斜線、垂直之間的分界。
        const float threshold = static_cast<float>(std::tan(M_PI / 8.0f));

        Point newEnd = start;
        if (ay <= ax * threshold) {
            // 水平線：固定 Y。
            newEnd.setX(start.x() + dx);
        } else if (ax <= ay * threshold) {
            // 垂直線：固定 X。
            newEnd.setY(start.y() + dy);
        } else {
            // 45° 斜線：投影到最近的對角線。
            const auto size = std::max(ax, ay);
            newEnd.setX(start.x() + (dx >= 0 ? size : -size));
            newEnd.setY(start.y() + (dy >= 0 ? size : -size));
        }

        this->_draft->setEnd(newEnd);
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

    ToolEventResult onClick(const ClickEvent& event, Point localPosition) override {
        beginDraft();
        _draft->setPosition(localPosition);
        return ToolEventResult::COMMIT;
    }
};

class PencilTool : public DrawingTool<Path> {
   public:
    using DrawingTool<Path>::DrawingTool;

    ToolEventResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override {
        this->beginDraft();
        _draft->addPoint(localPosition, true);

        return ToolEventResult::UPDATE;
    }

    ToolEventResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        _draft->addPoint(localPosition, true);

        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (!event.mouseState().isDown(MouseButton::MouseLeft)) {
            return ToolEventResult::NONE;
        }

        _draft->addPoint(localPosition);

        return ToolEventResult::UPDATE;
    }
};

class PolygonTool : public DrawingTool<Polygon> {
   public:
    using DrawingTool<Polygon>::DrawingTool;

    ToolEventResult onClick(const ClickEvent& event, Point localPosition) override {
        if (!_draft) {
            beginDraft();
        }

        // 第一次點擊時，加入第一個點；之後的點擊，更新最後一個點並加入新點。
        if (_draft->pointCount() == 0) {
            _draft->addPoint(localPosition);
        } else {
            _draft->setLastPoint(localPosition);
        }
        _draft->addPoint(localPosition);

        return ToolEventResult::UPDATE;
    }

    ToolEventResult onDoubleClick(const ClickEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        _draft->setLastPoint(localPosition);
        return ToolEventResult::COMMIT;
    }

    ToolEventResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (event.mouseState().isUp(MouseButton::MouseLeft) && _draft->pointCount() == 0) {
            // 尚未加入任何點，無法預覽。
            return ToolEventResult::CANCEL;
        }

        _draft->setLastPoint(localPosition);
        return ToolEventResult::UPDATE;
    }

    ToolEventResult onKeyDown(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return ToolEventResult::NONE;
        }

        if (event.key() == Key::Enter) {
            return ToolEventResult::COMMIT;
        }

        if (event.key() == Key::Escape) {
            return ToolEventResult::CANCEL;
        }

        if (event.key() == Key::Backspace) {
            if (_draft->pointCount() == 0) {
                return ToolEventResult::CANCEL;
            }
            _draft->removeLastPoint();
            return ToolEventResult::UPDATE;
        }

        return ToolEventResult::NONE;
    }
};

}  // namespace

const std::string getToolName(const Tool& tool) {
    switch (tool) {
    case Tool::TOOL_POINT:
        return "Point";
    case Tool::TOOL_PENCIL:
        return "Pencil";
    case Tool::TOOL_LINE:
        return "Line";
    case Tool::TOOL_RECTANGLE:
        return "Rectangle";
    case Tool::TOOL_ELLIPSE:
        return "Ellipse";
    case Tool::TOOL_POLYGON:
        return "Polygon";
    default:
        return "Unknown";
    }
}

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
