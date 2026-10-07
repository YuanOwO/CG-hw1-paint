#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class ColorFieldElement;
}

namespace paint::render {

// 負責繪製 ColorFieldElement 的 HSV 漸層與目前選取位置。
class ColorFieldRenderer {
   public:
    // 保存 OpenGL 狀態後依序繪製漸層與選取標記，結束時還原。
    void render(RenderContext& context, const ui::ColorFieldElement& field);

    // 以下兩者不保存 OpenGL 狀態，單獨呼叫時須由呼叫端自行 glPushAttrib / glPopAttrib。
    void renderPanel(RenderContext& context, const ui::ColorFieldElement& field);   // 飽和度 / 明度漸層
    void renderMarker(RenderContext& context, const ui::ColorFieldElement& field);  // 目前選取位置
};

}  // namespace paint::render
