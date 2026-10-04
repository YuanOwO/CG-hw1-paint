#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class ButtonElement;
}

namespace paint::render {

class ButtonRenderer {
   public:
    void render(RenderContext& context, const ui::ButtonElement& button);
};

}  // namespace paint::render
