#pragma once

#include <functional>
#include <utility>

#include "render/renderer/hue_slider_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

// 垂直色相滑桿：由上到下對應 0 到 360 度。
class HueSliderElement : public Element {
   public:
    using ValueChangedHandler = std::function<void(float hue)>;

    explicit HueSliderElement(float hue = 0.0f);

    float hue() const { return _hue; }
    void setHue(float hue);

    // 只有使用者操作會觸發 callback；程式呼叫 setHue() 時只更新顯示。
    void setOnValueChanged(ValueChangedHandler handler) { _onValueChanged = std::move(handler); }

   protected:
    Size measureContent(const Size& availableSize) override;
    void renderContent(render::RenderContext& context) override;

   private:
    render::HueSliderRenderer _renderer;
    ValueChangedHandler _onValueChanged;
    float _hue = 0.0f;

    // 將元件內的滑鼠位置換算成色相角度，並通知使用者操作。
    void updateFromPosition(Point position);
};

}  // namespace paint::ui
