#pragma once

#include "render/render_context.hpp"
#include "render/renderer/shape_renderer.hpp"

namespace paint::ui {
class CanvasElement;
}

namespace paint::render {

class CanvasRenderer {
   public:
    void render(RenderContext& context, const ui::CanvasElement& canvas);

   private:
    ShapeRenderer _shapeRenderer;
};

}  // namespace paint::render
