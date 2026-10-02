#include "ui/element.hpp"

#include <algorithm>

#include "common/point.hpp"
#include "event/events.hpp"
#include "ui/window.hpp"

namespace paint::ui {

#pragma region Geometry

void Element::setPreferredSize(const Size& size) {
    _preferredSize = size;
    invalidateLayout();
}

void Element::setMargin(const Margin& margin) {
    _margin = margin;
    invalidateLayout();
}

void Element::setPadding(const Padding& padding) {
    _padding = padding;
    invalidateLayout();
}

void Element::setHorizontalAlignment(Alignment alignment) {
    _horizontalAlignment = alignment;
    invalidateLayout();
}

void Element::setVerticalAlignment(Alignment alignment) {
    _verticalAlignment = alignment;
    invalidateLayout();
}

const Element* Element::hitTest(Point point) const {
    // 如果元素不可見、不可用，或者點不在元素範圍內，則返回 nullptr
    if (!isVisible() || !isEnabled() || !contains(point)) {
        return nullptr;
    }

    Point localPoint = point - _bounds.topLeft();

    // 先檢查子元素，從後往前檢查，確保 Z-order 正確
    for (auto it = children().rbegin(); it != children().rend(); it++) {
        Element* child = it->get();

        if (!child) {
            continue;  // 如果子元素不是 Element，則跳過
        }

        if (auto* target = child->hitTest(localPoint)) {
            return target;
        }
    }

    return this;
}

Size Element::measure(const Size& availableSize) {
    const auto paddingWidth = _padding.horizontal();
    const auto paddingHeight = _padding.vertical();

    Size contentAvailableSize = availableSize;

    // 減去 padding 後的可用大小
    if (contentAvailableSize.width) {
        contentAvailableSize.width = std::max(0, *contentAvailableSize.width - paddingWidth);
    }
    if (contentAvailableSize.height) {
        contentAvailableSize.height = std::max(0, *contentAvailableSize.height - paddingHeight);
    }

    const Size contentDesiredSize = measureContent(contentAvailableSize);

    // 檢查 measureContent() 返回的需求大小是否合法
    if (!contentDesiredSize.width.has_value() || !contentDesiredSize.height.has_value()) {
        throw std::runtime_error("measureContent() must return a concrete desired size");
    }
    if (*contentDesiredSize.width < 0 || *contentDesiredSize.height < 0) {
        throw std::runtime_error("measureContent() returned a negative desired size");
    }

    // 計算元素的最終需求大小，考慮 padding 與優先大小
    _desiredSize.width = std::max(_preferredSize.width.value_or(0), *contentDesiredSize.width + paddingWidth);
    _desiredSize.height =
        std::max(_preferredSize.height.value_or(0), *contentDesiredSize.height + paddingHeight);

    // 如果 availableSize 有限制，則將需求大小限制在 availableSize 內
    if (availableSize.width) {
        _desiredSize.width = std::min(*_desiredSize.width, *availableSize.width);
    }
    if (availableSize.height) {
        _desiredSize.height = std::min(*_desiredSize.height, *availableSize.height);
    }

    return _desiredSize;
}

void Element::arrange(const BoundingBox& bounds) {
    const auto oldBounds = _bounds;

    const bool moved = oldBounds.topLeft() != bounds.topLeft();
    const bool resized = oldBounds.width != bounds.width || oldBounds.height != bounds.height;

    _bounds = bounds;  // 記錄元素的邊界

    // 重新計算元素內的局域座標系，考慮 padding
    BoundingBox contentBounds{
        _padding.left,
        _padding.top,
        std::max(0, bounds.width - _padding.horizontal()),
        std::max(0, bounds.height - _padding.vertical()),
    };

    arrangeContent(contentBounds);

    if (moved) {
        ElementMoveEvent event(oldBounds.topLeft(), bounds.topLeft());
        dispatchEvent(event);
    }

    if (resized) {
        ElementResizeEvent event(bounds.width, bounds.height);
        dispatchEvent(event);
    }

    if (moved || resized) {
        invalidateDisplay();  // 標記元素需要重繪
    }
}

void Element::invalidateLayout() {
    if (auto* w = window()) {
        w->requestLayout();
    }
}

#pragma endregion  // Geometry

#pragma region State

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

    invalidateDisplay();
}

void Element::setEnabled(bool enabled) {
    if (_enabled == enabled) {
        return;
    }

    _enabled = enabled;
    invalidateDisplay();
}

void Element::setFocusable(bool focusable) {
    if (_focusable == focusable) {
        return;
    }

    _focusable = focusable;
    invalidateDisplay();
}

#pragma endregion  // State

#pragma region Tree

const Window* Element::window() const {
    const auto* current = this;

    // 往上走到根元素，然後返回其 window 指標
    while (current->_parent != nullptr) {
        current = current->_parent;
    }

    return current->_window;
}

Element& Element::appendChild(std::unique_ptr<Element> child) {
    if (!child) {
        throw std::invalid_argument("Child element cannot be null");
    }

    child->_parent = this;
    _children.push_back(std::move(child));

    invalidateLayout();

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

    // 必須在清除 parent 前通知 Window，才能辨識完整子樹並正常派送生命週期事件。
    if (auto* w = window()) {
        w->detachElementSubtree(child);
    }

    auto removedChild = std::move(*it);
    _children.erase(it);

    removedChild->_parent = nullptr;

    invalidateLayout();

    return removedChild;
}

EventTarget* Element::eventParent() const {
    if (_parent) {
        return _parent;
    }
    return _window;
}

#pragma endregion  // Tree

#pragma region Interaction

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

#pragma endregion  // Interaction

#pragma region Rendering

void Element::invalidateDisplay() {
    if (auto* w = window()) {
        w->requestRedisplay();
    }
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

#pragma endregion  // Rendering

}  // namespace paint::ui
