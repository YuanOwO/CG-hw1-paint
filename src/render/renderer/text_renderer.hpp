#pragma once

#include "render/render_context.hpp"

namespace paint::drawing {
class TextObject;
}
namespace paint::ui {
class TextElement;
}

namespace paint::render {

class TextRenderer {
   public:
    void render(RenderContext& context, const ui::TextElement& text);
    void draw(RenderContext& context, const drawing::TextObject& text);
};

}  // namespace paint::render
