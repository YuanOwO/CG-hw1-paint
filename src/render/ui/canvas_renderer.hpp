#pragma once

#include "render/render_context.hpp"
#include "render/shape_renderer.hpp"
namespace paint {

class CanvasElement;

class CanvasRenderer {
   public:
    void render(RenderContext& context, const CanvasElement& canvas);

   private:
    ShapeRenderer _shapeRenderer;
};

}  // namespace paint
