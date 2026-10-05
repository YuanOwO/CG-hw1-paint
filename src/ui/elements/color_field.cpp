#include "ui/elements/color_field.hpp"

#include <algorithm>
#include <utility>

#include "event/events.hpp"

namespace paint::ui {

ColorFieldElement::ColorFieldElement(const ColorHSV& color) {
    setPreferredSize({280, 220});
    setColor(color);

    // 按下滑鼠後捕獲輸入，讓拖曳超出元件範圍時仍可持續調整。
    addEventListener<MouseDownEvent>([this](MouseDownEvent& event) {
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }

        captureMouse();
        updateFromPosition(windowToLocal(event.position()));
        event.stopPropagation();
    });

    addEventListener<MouseMoveEvent>([this](MouseMoveEvent& event) {
        if (!event.mouseState().isDown(MouseButton::MouseLeft)) {
            return;
        }

        updateFromPosition(windowToLocal(event.position()));
        event.stopPropagation();
    });

    addEventListener<MouseUpEvent>([this](MouseUpEvent& event) {
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }

        updateFromPosition(windowToLocal(event.position()));
        releaseMouseCapture();
        event.stopPropagation();
    });
}

void ColorFieldElement::setHue(float hue) {
    const float normalized = normalizeHue(hue);
    if (_hue == normalized) {
        return;
    }

    _hue = normalized;
    invalidateDisplay();
}

void ColorFieldElement::setSaturation(float saturation) {
    const float clamped = std::clamp(saturation, 0.0f, 1.0f);
    if (_saturation == clamped) {
        return;
    }

    _saturation = clamped;
    invalidateDisplay();
}

void ColorFieldElement::setValue(float value) {
    const float clamped = std::clamp(value, 0.0f, 1.0f);
    if (_value == clamped) {
        return;
    }

    _value = clamped;
    invalidateDisplay();
}

void ColorFieldElement::setColor(const ColorHSV& color) {
    _hue = normalizeHue(color.h);
    _saturation = std::clamp(color.s, 0.0f, 1.0f);
    _value = std::clamp(color.v, 0.0f, 1.0f);
    invalidateDisplay();
}

Size ColorFieldElement::measureContent(const Size&) {
    return {280, 220};
}

void ColorFieldElement::renderContent(render::RenderContext& context) {
    _renderer.render(context, *this);
}

void ColorFieldElement::updateFromPosition(Point position) {
    if (width() <= 1 || height() <= 1) {
        return;
    }

    // 左到右對應低到高飽和度；上到下對應高到低明度。
    const float saturation = std::clamp(position.x() / static_cast<float>(width() - 1), 0.0f, 1.0f);
    const float value =
        1.0f - std::clamp(position.y() / static_cast<float>(height() - 1), 0.0f, 1.0f);

    if (_saturation == saturation && _value == value) {
        return;
    }

    _saturation = saturation;
    _value = value;
    invalidateDisplay();

    if (_onValueChanged) {
        _onValueChanged(_saturation, _value);
    }
}

}  // namespace paint::ui
