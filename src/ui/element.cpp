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

Point Element::windowToLocal(Point point) const {
    auto current = this;
    while (current != nullptr) {
        point = point - current->_bounds.topLeft();
        current = current->_parent;
    }

    return point;
}

Point Element::localToWindow(Point point) const {
    auto current = this;
    while (current != nullptr) {
        point = point + current->_bounds.topLeft();
        current = current->_parent;
    }

    return point;
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
    const auto marginWidth = _margin.horizontal();
    const auto marginHeight = _margin.vertical();
    const auto paddingWidth = _padding.horizontal();
    const auto paddingHeight = _padding.vertical();

    // 1. 扣掉 margin，得到元素可用大小
    Size elementAvailableSize = availableSize;

    if (elementAvailableSize.width.has_value()) {
        elementAvailableSize.width = std::max(0, *elementAvailableSize.width - marginWidth);
    }

    if (elementAvailableSize.height.has_value()) {
        elementAvailableSize.height = std::max(0, *elementAvailableSize.height - marginHeight);
    }

    // 2. 扣掉 padding，得到內容可用大小
    Size contentAvailableSize = elementAvailableSize;

    if (contentAvailableSize.width.has_value()) {
        contentAvailableSize.width = std::max(0, *contentAvailableSize.width - paddingWidth);
    }

    if (contentAvailableSize.height.has_value()) {
        contentAvailableSize.height = std::max(0, *contentAvailableSize.height - paddingHeight);
    }

    // 3. 由子類別量測內容
    const Size contentDesiredSize = measureContent(contentAvailableSize);

    if (!contentDesiredSize.width.has_value() || !contentDesiredSize.height.has_value()) {
        throw std::runtime_error("measureContent() must return a concrete desired size");
    }

    if (*contentDesiredSize.width < 0 || *contentDesiredSize.height < 0) {
        throw std::runtime_error("measureContent() returned a negative desired size");
    }

    // 4. 加回 padding，並考慮 preferredSize。
    int width = std::max(_preferredSize.width.value_or(0), *contentDesiredSize.width + _padding.horizontal());

    int height =
        std::max(_preferredSize.height.value_or(0), *contentDesiredSize.height + _padding.vertical());

    // 5. 本體需求不能超過本體可用大小。
    if (elementAvailableSize.width.has_value()) {
        width = std::min(width, *elementAvailableSize.width);
    }

    if (elementAvailableSize.height.has_value()) {
        height = std::min(height, *elementAvailableSize.height);
    }

    _desiredElementSize = {width, height};

    // 6. 加回 margin，得到要回報父容器的占用大小。
    int outerWidth = std::max(0, width + _margin.horizontal());
    int outerHeight = std::max(0, height + _margin.vertical());

    if (availableSize.width.has_value()) {
        outerWidth = std::min(outerWidth, *availableSize.width);
    }

    if (availableSize.height.has_value()) {
        outerHeight = std::min(outerHeight, *availableSize.height);
    }

    _desiredSize = {outerWidth, outerHeight};
    return _desiredSize;
}

void Element::arrange(const BoundingBox& slot) {
    const auto oldBounds = _bounds;

    // 1. 扣除 margin，得到本體可用區域。
    //    此時仍使用父元素的座標系。
    const int availableX = slot.x + _margin.left;
    const int availableY = slot.y + _margin.top;

    const int availableWidth = std::max(0, slot.width - _margin.horizontal());

    const int availableHeight = std::max(0, slot.height - _margin.vertical());

    // 2. Stretch 填滿可用大小；
    //    其他對齊使用量測大小，但不能超過可用大小。
    const int width = _horizontalAlignment == Alignment::Stretch
                          ? availableWidth
                          : std::min(*_desiredElementSize.width, availableWidth);

    const int height = _verticalAlignment == Alignment::Stretch
                           ? availableHeight
                           : std::min(*_desiredElementSize.height, availableHeight);

    // 3. 根據 alignment 決定位置。
    int x = availableX;
    int y = availableY;

    if (_horizontalAlignment == Alignment::Center) {
        x += (availableWidth - width) / 2;
    } else if (_horizontalAlignment == Alignment::End) {
        x += availableWidth - width;
    }

    if (_verticalAlignment == Alignment::Center) {
        y += (availableHeight - height) / 2;
    } else if (_verticalAlignment == Alignment::End) {
        y += availableHeight - height;
    }

    // bounds 的位置相對父元素，大小不包含 margin。
    _bounds = {x, y, width, height};

    // 4. 內容區域改用自己的局部座標。
    //    自己的左上角是 (0, 0)，因此不用再加 x、y。
    const BoundingBox contentBounds{
        _padding.left,
        _padding.top,
        std::max(0, width - _padding.horizontal()),
        std::max(0, height - _padding.vertical()),
    };

    arrangeContent(contentBounds);

    // 5. 根據實際 bounds 的變化發送事件。
    const bool moved = oldBounds.topLeft() != _bounds.topLeft();
    const bool resized = oldBounds.width != width || oldBounds.height != height;

    if (moved) {
        ElementMoveEvent event(oldBounds.topLeft(), _bounds.topLeft());
        dispatchEvent(event);
    }

    if (resized) {
        ElementResizeEvent event(width, height);
        dispatchEvent(event);
    }

    if (moved || resized) {
        invalidateDisplay();
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

void Element::setBackgroundColor(const ColorRGBA& color) {
    if (_backgroundColor == color) {
        return;
    }
    _backgroundColor = color;
    invalidateDisplay();
}

void Element::invalidateDisplay() {
    if (auto* w = window()) {
        w->requestRedisplay();
    }
}

void Element::render(render::RenderContext& context) {
    if (!isVisible()) {
        return;
    }

    context.pushTransform();  // 保存父元素的座標系

    context.translate(static_cast<float>(x()), static_cast<float>(y()));  // 將原點移動到目前元素的局部座標

    context.fillRect(width(), height(), _backgroundColor);
    renderContent(context);

    for (const auto& child : children()) {
        if (auto* it = dynamic_cast<Element*>(child.get())) {
            it->render(context);
        }
    }

    // Caret 與 selection 這類前景效果應蓋在子元件上方。
    renderOverlay(context);

    context.popTransform();  // 恢復父元素的座標系
}

void Element::updateTree(std::chrono::milliseconds delta) {
    update(delta);

    for (const auto& child : _children) {
        child->updateTree(delta);
    }
}

#pragma endregion  // Rendering

}  // namespace paint::ui
