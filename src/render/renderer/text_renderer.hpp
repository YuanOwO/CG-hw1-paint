#pragma once

#include <string>

#include "common/color.hpp"
#include "common/font.hpp"
#include "common/point.hpp"
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

   private:
    static void drawText(RenderContext& context, const std::string& text,
                         const FontStyle& fontStyle, const ColorRGBA& color,
                         const Point& origin);
};

}  // namespace paint::render
