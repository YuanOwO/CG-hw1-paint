#pragma once

#include "render/render_context.hpp"
#include "render/shape_renderer.hpp"

namespace paint::ui {

class CanvasElement;

}  // namespace paint::ui

namespace paint {

class CanvasRenderer {
   public:
    void render(RenderContext& context, const ui::CanvasElement& canvas);

   private:
    ShapeRenderer _shapeRenderer;
};

}  // namespace paint
