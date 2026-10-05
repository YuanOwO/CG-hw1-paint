#include "drawing/tools/shape_tools.hpp"

#include <algorithm>
#include <cmath>

namespace paint::drawing {

#pragma region DragTool

// 直線、矩形、圓形等需要拖曳兩個點的工具可以共用這個基底類別。

template <typename TShape>
ToolResult DragTool<TShape>::onKeyDown(const KeyboardEvent& event, Point localPosition) {
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

template <typename TShape>
ToolResult DragTool<TShape>::onKeyUp(const KeyboardEvent& event, Point localPosition) {
    if (!this->_draft) {
        return {};
    }

    if (isShiftKey(event.key())) {
        this->_draft->setEnd(localPosition);
        return ToolResult::redraw();
    }

    return {};
}

template <typename TShape>
ToolResult DragTool<TShape>::onMouseDown(const MouseButtonEvent& event, Point localPosition) {
    this->beginShapeDraft();
    this->_draft->setStart(localPosition);
    this->_draft->setEnd(localPosition);
    return ToolResult::redraw();
}

template <typename TShape>
ToolResult DragTool<TShape>::onMouseUp(const MouseButtonEvent& event, Point localPosition) {
    if (!this->_draft) {
        return {};
    }

    this->_draft->setEnd(localPosition);
    if (event.keyboardState().isShiftDown()) {
        onShift(event, localPosition);
    }

    return this->commitDraft();
}

template <typename TShape>
ToolResult DragTool<TShape>::onMouseMove(const MouseMoveEvent& event, Point localPosition) {
    if (!this->_draft || !event.mouseState().isDown(MouseButton::MouseLeft)) {
        return {};
    }

    this->_draft->setEnd(localPosition);
    if (event.keyboardState().isShiftDown()) {
        onShift(event, localPosition);
    }

    return ToolResult::redraw();
}

template <typename TShape>
void DragTool<TShape>::onShift(const InputEvent& event, Point localPosition) {
    // 預設行為：畫出正圓、正方形。
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

// DragTool 的實作位於此 .cpp，因此必須明確產生各個實際使用型別的模板符號。
template class DragTool<LineShape>;
template class DragTool<RectangleShape>;
template class DragTool<EllipseShape>;

#pragma endregion  // DragTool

#pragma region LineTool

void LineTool::onShift(const InputEvent& event, Point localPosition) {
    // 直線工具的行為：畫出水平、垂直或 45° 斜線。
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

#pragma endregion  // LineTool

#pragma region PointTool

ToolResult PointTool::onClick(const ClickEvent& event, Point localPosition) {
    beginShapeDraft();
    this->_draft->setPosition(localPosition);
    return this->commitDraft();
}

#pragma endregion  // PointTool

#pragma region PencilTool

ToolResult PencilTool::onMouseDown(const MouseButtonEvent& event, Point localPosition) {
    this->beginShapeDraft();
    this->_draft->addPoint(localPosition, true);
    return ToolResult::redraw();
}

ToolResult PencilTool::onMouseUp(const MouseButtonEvent& event, Point localPosition) {
    if (!this->_draft) {
        return {};
    }

    this->_draft->addPoint(localPosition, true);
    return this->commitDraft();
}

ToolResult PencilTool::onMouseMove(const MouseMoveEvent& event, Point localPosition) {
    if (!this->_draft || !event.mouseState().isDown(MouseButton::MouseLeft)) {
        return {};
    }

    this->_draft->addPoint(localPosition);
    return ToolResult::redraw();
}

#pragma endregion  // PencilTool

#pragma region PolygonTool

ToolResult PolygonTool::onKeyDown(const KeyboardEvent& event, Point localPosition) {
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

ToolResult PolygonTool::onClick(const ClickEvent& event, Point localPosition) {
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

ToolResult PolygonTool::onDoubleClick(const ClickEvent& event, Point localPosition) {
    if (!this->_draft) {
        return {};
    }

    this->_draft->setLastPoint(localPosition);
    return this->commitDraft();
}

ToolResult PolygonTool::onMouseMove(const MouseMoveEvent& event, Point localPosition) {
    if (!this->_draft) {
        return {};
    }

    if (event.mouseState().isUp(MouseButton::MouseLeft) && this->_draft->pointCount() == 0) {
        return this->cancelDraft();
    }

    this->_draft->setLastPoint(localPosition);
    return ToolResult::redraw();
}

#pragma endregion  // PolygonTool

}  // namespace paint::drawing
