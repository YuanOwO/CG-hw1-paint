#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class HueSliderElement;
}

namespace paint::render {

// 負責繪製 HueSliderElement 的完整色相漸層與位置指示器。
class HueSliderRenderer {
   public:
    void render(RenderContext& context, const ui::HueSliderElement& slider);
};

}  // namespace paint::render
