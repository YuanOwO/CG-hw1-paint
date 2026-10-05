#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class ColorFieldElement;
}

namespace paint::render {

// 負責繪製 ColorFieldElement 的 HSV 漸層與目前選取位置。
class ColorFieldRenderer {
   public:
    void render(RenderContext& context, const ui::ColorFieldElement& field);
};

}  // namespace paint::render
