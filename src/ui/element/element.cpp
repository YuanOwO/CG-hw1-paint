#include "ui/element/element.hpp"

#include <algorithm>

#include "app/window.hpp"
#include "common/point.hpp"

namespace paint {

Element& Element::appendChild(std::unique_ptr<Element> child) {
    if (!child) {
        throw std::invalid_argument("Child element cannot be null");
    }

    child->_parent = this;
    _children.push_back(std::move(child));

    invalidate();  // 通知父視窗需要重新渲染

    return *_children.back();
}

std::unique_ptr<Element> Element::removeChild(Element* child) {
    if (!child) {
        throw std::invalid_argument("Child element cannot be null");
    }

    auto it = std::find_if(_children.begin(), _children.end(),
                           [child](const std::unique_ptr<Element>& ptr) { return ptr.get() == child; });

    if (it == _children.end()) {
        throw std::invalid_argument("Child element not found");
    }

    std::unique_ptr<Element> removedChild = std::move(*it);
    _children.erase(it);

    removedChild->_parent = nullptr;

    invalidate();  // 通知父視窗需要重新渲染

    return removedChild;
}

const Window* Element::window() const {
    const Element* current = this;

    // 往上走到根元素，然後返回其 window 指標
    while (current->_parent != nullptr) {
        current = current->_parent;
    }

    return current->_window;
}

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
    for (auto it = _children.rbegin(); it != _children.rend(); it++) {
        Element* child = it->get();

        if (auto* target = child->hitTest(localPoint)) {
            return target;
        }
    }

    return this;
}

EventTarget* Element::eventParent() const {
    if (_parent) {
        return _parent;
    }
    return _window;
}

void Element::render(RenderContext& context) {
    if (!isVisible()) {
        return;
    }

    context.pushTransform();  // 保存父元素的座標系

    context.translate(static_cast<float>(x()), static_cast<float>(y()));  // 將原點移動到目前元素的局部座標

    renderContent(context);

    for (const auto& child : _children) {
        child->render(context);
    }

    context.popTransform();  // 恢復父元素的座標系
}

}  // namespace paint
