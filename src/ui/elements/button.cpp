#include "ui/elements/button.hpp"

#include "event/events.hpp"

namespace paint::ui {

ButtonElement::ButtonElement(ClickHandler onClick) : _onClick(std::move(onClick)) {
    setPadding({8, 4, 8, 4});
    setFocusable(true);

    addEventListener<HoverEvent>([this](HoverEvent& event) {
        _hovered = true;
        invalidateDisplay();
    });

    addEventListener<UnhoverEvent>([this](UnhoverEvent& event) {
        _hovered = false;
        invalidateDisplay();
    });

    addEventListener<MouseDownEvent>([this](MouseDownEvent& event) {
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }

        _pressed = true;
        captureMouse();
        invalidateDisplay();
    });

    addEventListener<MouseUpEvent>([this](MouseUpEvent& event) {
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }

        _pressed = false;
        releaseMouseCapture();
        invalidateDisplay();
    });

    addEventListener<MouseMoveEvent>([this](MouseMoveEvent& event) {
        // 如果滑鼠按下，則按鈕保持按下狀態，直到滑鼠釋放。
        if (event.mouseState().isDown(MouseButton::MouseLeft)) {
            return;
        }

        // 如果滑鼠移動到按鈕外部，則取消按下狀態。
        if (!contains(event.position())) {
            _pressed = false;
            invalidateDisplay();
        }
    });

    addEventListener<ClickEvent>([this](ClickEvent& event) {
        if (event.button() == MouseButton::MouseLeft && _onClick) {
            _onClick();
        }
    });

    // 因為 ClickEvent 只會在滑鼠按下和釋放時觸發一次，所以我們需要額外處理 DoubleClickEvent。
    addEventListener<DoubleClickEvent>([this](DoubleClickEvent& event) {
        if (event.button() == MouseButton::MouseLeft && _onClick) {
            _onClick();
        }
    });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (event.key() == Key::Enter && _onClick) {
            _onClick();
        }
    });
}

Size ButtonElement::measureContent(const Size& availableSize) {
    if (children().empty()) {
        return {0, 0};
    }

    const auto& child = children().front();
    return child->measure(availableSize);
}

void ButtonElement::arrangeContent(const BoundingBox& contentBounds) {
    if (children().empty()) {
        return;
    }

    const auto& child = children().front();
    child->arrange(contentBounds);
}

void ButtonElement::renderContent(RenderContext& context) {
    _renderer.render(context, *this);
}

}  // namespace paint::ui
