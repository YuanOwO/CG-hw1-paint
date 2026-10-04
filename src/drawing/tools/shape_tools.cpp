#include <algorithm>
#include <cmath>

#include "app/application.hpp"
#include "app/windows/input_dialog.hpp"
#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"
#include "drawing/tools/creation_tool.hpp"
#include "drawing/tools/tool_factory.hpp"

namespace paint::drawing {
namespace {

// 直線、矩形、圓形等需要拖曳兩個點的工具可以共用這個基底類別。
template <typename TShape>
class DragTool : public ShapeCreationTool<TShape> {
   public:
    using ShapeCreationTool<TShape>::ShapeCreationTool;

    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (event.key() == Key::Escape) {
            return this->cancelDraft(true);
        }

        if (isShiftKey(event.key())) {
            onShift(event, localPosition);
            return ToolResult::redraw();
        }

        return {};
    }

    ToolResult onKeyUp(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (isShiftKey(event.key())) {
            this->_draft->setEnd(localPosition);
            return ToolResult::redraw();
        }

        return {};
    }

    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override {
        this->beginShapeDraft();
        this->_draft->setStart(localPosition);
        this->_draft->setEnd(localPosition);
        return ToolResult::redraw();
    }

    ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        this->_draft->setEnd(localPosition);
        if (event.keyboardState().isShiftDown()) {
            onShift(event, localPosition);
        }

        return this->commitDraft();
    }

    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft || !event.mouseState().isDown(MouseButton::MouseLeft)) {
            return {};
        }

        this->_draft->setEnd(localPosition);
        if (event.keyboardState().isShiftDown()) {
            onShift(event, localPosition);
        }

        return ToolResult::redraw();
    }

   protected:
    // Shift 鍵被按下時，會畫出正方形、正圓或 45° 斜線。
    virtual void onShift(const InputEvent& event, Point localPosition) {
        const Point& start = this->_draft->start();
        const Point& now = localPosition;

        const float dx = now.x() - start.x();
        const float dy = now.y() - start.y();
        const auto size = std::max(std::abs(dx), std::abs(dy));

        Point newEnd;
        newEnd.setX(start.x() + (dx >= 0 ? size : -size));
        newEnd.setY(start.y() + (dy >= 0 ? size : -size));
        this->_draft->setEnd(newEnd);
    }
};

class LineTool : public DragTool<LineShape> {
   public:
    using DragTool<LineShape>::DragTool;

   protected:
    // Shift 鍵被按下時，會畫出水平、垂直或 45° 斜線。
    void onShift(const InputEvent& event, Point localPosition) override {
        const Point& start = this->_draft->start();
        const Point& now = localPosition;

        const float dx = now.x() - start.x();
        const float dy = now.y() - start.y();
        const float ax = std::abs(dx);
        const float ay = std::abs(dy);
        const float threshold = static_cast<float>(std::tan(M_PI / 8.0f));

        Point newEnd = start;
        if (ay <= ax * threshold) {
            newEnd.setX(start.x() + dx);
        } else if (ax <= ay * threshold) {
            newEnd.setY(start.y() + dy);
        } else {
            const auto size = std::max(ax, ay);
            newEnd.setX(start.x() + (dx >= 0 ? size : -size));
            newEnd.setY(start.y() + (dy >= 0 ? size : -size));
        }

        this->_draft->setEnd(newEnd);
    }
};

class RectangleTool : public DragTool<RectangleShape> {
   public:
    using DragTool<RectangleShape>::DragTool;
};

class EllipseTool : public DragTool<EllipseShape> {
   public:
    using DragTool<EllipseShape>::DragTool;
};

class PointTool : public ShapeCreationTool<PointShape> {
   public:
    using ShapeCreationTool<PointShape>::ShapeCreationTool;

    ToolResult onClick(const ClickEvent& event, Point localPosition) override {
        beginShapeDraft();
        _draft->setPosition(localPosition);
        return commitDraft();
    }
};

class PencilTool : public ShapeCreationTool<PathShape> {
   public:
    using ShapeCreationTool<PathShape>::ShapeCreationTool;

    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override {
        this->beginShapeDraft();
        _draft->addPoint(localPosition, true);
        return ToolResult::redraw();
    }

    ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        _draft->addPoint(localPosition, true);
        return commitDraft();
    }

    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft || !event.mouseState().isDown(MouseButton::MouseLeft)) {
            return {};
        }

        _draft->addPoint(localPosition);
        return ToolResult::redraw();
    }
};

class PolygonTool : public ShapeCreationTool<PolygonShape> {
   public:
    using ShapeCreationTool<PolygonShape>::ShapeCreationTool;

    ToolResult onClick(const ClickEvent& event, Point localPosition) override {
        if (!_draft) {
            beginShapeDraft();
        }

        if (_draft->pointCount() == 0) {
            _draft->addPoint(localPosition);
        } else {
            _draft->setLastPoint(localPosition);
        }
        _draft->addPoint(localPosition);

        return ToolResult::redraw();
    }

    ToolResult onDoubleClick(const ClickEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        _draft->setLastPoint(localPosition);
        return commitDraft();
    }

    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (event.mouseState().isUp(MouseButton::MouseLeft) && _draft->pointCount() == 0) {
            return cancelDraft();
        }

        _draft->setLastPoint(localPosition);
        return ToolResult::redraw();
    }

    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (event.key() == Key::Enter) {
            return commitDraft(true);
        }

        if (event.key() == Key::Escape) {
            return cancelDraft(true);
        }

        if (event.key() == Key::Backspace) {
            if (_draft->pointCount() == 0) {
                return cancelDraft(true);
            }
            _draft->removeLastPoint();
            return ToolResult::redraw(true);
        }

        return {};
    }
};

}  // namespace

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

std::unique_ptr<ICanvasTool> createCanvasTool(ToolKind tool, ShapeStyle style) {
    switch (tool) {
    case ToolKind::POINT:
        return std::make_unique<PointTool>(style);
    case ToolKind::PENCIL:
        return std::make_unique<PencilTool>(style);
    case ToolKind::LINE:
        return std::make_unique<LineTool>(style);
    case ToolKind::RECTANGLE:
        return std::make_unique<RectangleTool>(style);
    case ToolKind::ELLIPSE:
        return std::make_unique<EllipseTool>(style);
    case ToolKind::POLYGON:
        return std::make_unique<PolygonTool>(style);
    default:
        return nullptr;
    }
}

}  // namespace paint::drawing
