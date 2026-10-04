#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class InputElement;
}

namespace paint::render {

class InputRenderer {
   public:
    void render(RenderContext& context, const ui::InputElement& input);
    void renderCursor(RenderContext& context, const ui::InputElement& input);
};

}  // namespace paint::render
