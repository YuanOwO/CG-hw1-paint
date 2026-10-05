#pragma once

#include <functional>
#include <utility>

#include "common/color.hpp"
#include "render/renderer/color_field_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

// 二維 HSV 選色區：水平方向控制飽和度，垂直方向控制明度。
class ColorFieldElement : public Element {
   public:
    using ValueChangedHandler = std::function<void(float saturation, float value)>;

    explicit ColorFieldElement(const ColorHSV& color = ColorHSV{0.0f, 0.0f, 0.0f});

    float hue() const { return _hue; }
    float saturation() const { return _saturation; }
    float value() const { return _value; }

    void setHue(float hue);
    void setSaturation(float saturation);
    void setValue(float value);
    void setColor(const ColorHSV& color);

    // 只有使用者操作會觸發 callback；程式呼叫 setter 時不會觸發，
    // 避免與 RGB、Hex 等其他控制項同步時形成循環更新。
    void setOnValueChanged(ValueChangedHandler handler) { _onValueChanged = std::move(handler); }

   protected:
    Size measureContent(const Size& availableSize) override;
    void renderContent(render::RenderContext& context) override;

   private:
    render::ColorFieldRenderer _renderer;
    ValueChangedHandler _onValueChanged;

    float _hue = 0.0f;
    float _saturation = 0.0f;
    float _value = 0.0f;

    // 將元件內的滑鼠位置換算成 saturation/value，並通知使用者操作。
    void updateFromPosition(Point position);
};

}  // namespace paint::ui
