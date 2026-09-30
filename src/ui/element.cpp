#include "ui/element.hpp"

#include <algorithm>

#include "common/point.hpp"
#include "ui/window.hpp"

namespace paint::ui {

void Element::setVisible(bool visible) {
    if (_visible == visible) {
        return;
    }

    _visible = visible;

    if (_visible) {
        ElementVisibleEvent event;
        dispatchEvent(event);
    } else {
        ElementHiddenEvent event;
        dispatchEvent(event);
    }

    invalidate();
}

void Element::setEnabled(bool enabled) {
    if (_enabled == enabled) {
        return;
    }

    _enabled = enabled;
    invalidate();
}

void Element::setFocusable(bool focusable) {
    if (_focusable == focusable) {
        return;
    }

    _focusable = focusable;
    invalidate();
}

void Element::setBounds(const BoundingBox& bounds) {
    if (bounds.width < 0 || bounds.height < 0) {
        throw std::invalid_argument("Element size cannot be negative");
    }

    const auto oldBounds = _bounds;

    if (oldBounds == bounds) {
        return;  // 如果邊界沒有改變，則不需要做任何操作
    }

    _bounds = bounds;

    if (oldBounds.topLeft() != bounds.topLeft()) {
        ElementMoveEvent moveEvent(oldBounds.topLeft(), bounds.topLeft());
        dispatchEvent(moveEvent);
    }

    if (oldBounds.width != bounds.width || oldBounds.height != bounds.height) {
        ElementResizeEvent resizeEvent(bounds.width, bounds.height);
        dispatchEvent(resizeEvent);
    }

    invalidate();
}

void Element::invalidate() {
    if (auto* w = window()) {
        w->requestRedisplay();
    }
}

void Element::captureMouse() {
    if (auto* w = window()) {
        w->captureMouse(this);
    }
}

void Element::releaseMouseCapture() {
    if (auto* w = window()) {
        w->releaseMouseCaptureIf(this);
    }
}

const Element* Element::hitTest(Point point) const {
    // 如果元素不可見、不可用，或者點不在元素範圍內，則返回 nullptr
    if (!isVisible() || !isEnabled() || !contains(point)) {
        return nullptr;
    }

    Point localPoint = point - _bounds.topLeft();

    // 先檢查子元素，從後往前檢查，確保 Z-order 正確
    for (auto it = children().rbegin(); it != children().rend(); it++) {
        Element* child = dynamic_cast<Element*>(it->get());

        if (!child) {
            continue;  // 如果子元素不是 Element，則跳過
        }

        if (auto* target = child->hitTest(localPoint)) {
            return target;
        }
    }

    return this;
}

void Element::render(RenderContext& context) {
    if (!isVisible()) {
        return;
    }

    context.pushTransform();  // 保存父元素的座標系

    context.translate(static_cast<float>(x()), static_cast<float>(y()));  // 將原點移動到目前元素的局部座標

    renderContent(context);

    for (const auto& child : children()) {
        if (auto* it = dynamic_cast<Element*>(child.get())) {
            it->render(context);
        }
    }

    context.popTransform();  // 恢復父元素的座標系
}

}  // namespace paint::ui
