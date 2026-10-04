#pragma once

#include <algorithm>
#include <cmath>

#include "drawing/shape_object.hpp"
#include "drawing/tools/creation_tool.hpp"

namespace paint::drawing {

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
        this->_draft->setPosition(localPosition);
        return this->commitDraft();
    }
};

class PencilTool : public ShapeCreationTool<PathShape> {
   public:
    using ShapeCreationTool<PathShape>::ShapeCreationTool;

    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override {
        this->beginShapeDraft();
        this->_draft->addPoint(localPosition, true);
        return ToolResult::redraw();
    }

    ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        this->_draft->addPoint(localPosition, true);
        return this->commitDraft();
    }

    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft || !event.mouseState().isDown(MouseButton::MouseLeft)) {
            return {};
        }

        this->_draft->addPoint(localPosition);
        return ToolResult::redraw();
    }
};

class PolygonTool : public ShapeCreationTool<PolygonShape> {
   public:
    using ShapeCreationTool<PolygonShape>::ShapeCreationTool;

    ToolResult onClick(const ClickEvent& event, Point localPosition) override {
        if (!this->_draft) {
            this->beginShapeDraft();
        }

        if (this->_draft->pointCount() == 0) {
            this->_draft->addPoint(localPosition);
        } else {
            this->_draft->setLastPoint(localPosition);
        }
        this->_draft->addPoint(localPosition);

        return ToolResult::redraw();
    }

    ToolResult onDoubleClick(const ClickEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        this->_draft->setLastPoint(localPosition);
        return this->commitDraft();
    }

    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (event.mouseState().isUp(MouseButton::MouseLeft) && this->_draft->pointCount() == 0) {
            return this->cancelDraft();
        }

        this->_draft->setLastPoint(localPosition);
        return ToolResult::redraw();
    }

    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override {
        if (!this->_draft) {
            return {};
        }

        if (event.key() == Key::Enter) {
            return this->commitDraft(true);
        }

        if (event.key() == Key::Escape) {
            return this->cancelDraft(true);
        }

        if (event.key() == Key::Backspace) {
            if (this->_draft->pointCount() == 0) {
                return this->cancelDraft(true);
            }
            this->_draft->removeLastPoint();
            return ToolResult::redraw(true);
        }

        return {};
    }
};

}  // namespace paint::drawing
