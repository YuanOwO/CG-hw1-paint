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

Window* Element::window() const {
    const Element* current = this;

    // 往上走到根元素，然後返回其 window 指標
    while (current->_parent != nullptr) {
        current = current->_parent;
    }

    return current->_window;
}

EventTarget* Element::eventParent() const {
    if (_parent) {
        return _parent;
    }
    return _window;
}

void Element::setBounds(const BoundingBox& bounds) {
    if (bounds.width < 0 || bounds.height < 0) {
        throw std::invalid_argument("Element size cannot be negative");
    }
    _bounds = bounds;
    invalidate();
}

void Element::invalidate() {
    if (auto* w = window()) {
        w->requestRedisplay();
    }
}

Element* Element::hitTest(Point point) {
    // 如果沒命中自己，一切免談
    if (!contains(point)) {
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

bool Element::contains(Point point) const {
    return _bounds.contains(point);
}

}  // namespace paint
