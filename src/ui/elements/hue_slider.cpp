#include "ui/elements/hue_slider.hpp"

#include <algorithm>

#include "common/color.hpp"
#include "event/events.hpp"

namespace paint::ui {

HueSliderElement::HueSliderElement(float hue) {
    setHue(hue);

    // 捕獲滑鼠可避免拖曳到滑桿外時中斷操作。
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

void HueSliderElement::setHue(float hue) {
    const float normalized = normalizeHue(hue);
    if (_hue == normalized) {
        return;
    }

    _hue = normalized;
    invalidateDisplay();
}

Size HueSliderElement::measureContent(const Size&) {
    return {30, 220};
}

void HueSliderElement::renderContent(render::RenderContext& context) {
    _renderer.render(context, *this);
}

void HueSliderElement::updateFromPosition(Point position) {
    if (height() <= 1) {
        return;
    }

    // UI 使用由上往下增加的 Y 座標，因此可直接映射到 0～360 度。
    const float ratio = std::clamp(position.y() / static_cast<float>(height()), 0.0f, 1.0f);
    // 360 度與 0 度相同；底端保留在 360 以下，避免指示器跳回頂端。
    const float hue = ratio == 1.0f ? 359.999f : ratio * 360.0f;
    if (_hue == hue) {
        return;
    }

    _hue = hue;
    invalidateDisplay();

    if (_onValueChanged) {
        _onValueChanged(_hue);
    }
}

}  // namespace paint::ui
