#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class HueSliderElement;
}

namespace paint::render {

// 負責繪製 HueSliderElement 的色相漸層與目前色相指示器。
class HueSliderRenderer {
   public:
    // 保存 OpenGL 狀態後依序繪製漸層與指示器，結束時還原。
    void render(RenderContext& context, const ui::HueSliderElement& slider);

    // 以下兩者不保存 OpenGL 狀態，單獨呼叫時須由呼叫端自行 glPushAttrib / glPopAttrib。
    void renderPanel(RenderContext& context, const ui::HueSliderElement& slider);   // 色相漸層
    void renderMarker(RenderContext& context, const ui::HueSliderElement& slider);  // 目前色相指示器
};

}  // namespace paint::render
