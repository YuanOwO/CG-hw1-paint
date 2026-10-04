#pragma once

#include "render/render_context.hpp"

namespace paint::ui {
class TextElement;
}

namespace paint::render {

class TextRenderer {
   public:
    void render(RenderContext& context, const ui::TextElement& text);
};

}  // namespace paint::render
