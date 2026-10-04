#pragma once

#include "render/render_context.hpp"
#include "render/renderer/scene_renderer.hpp"

namespace paint::ui {
class CanvasElement;
}

namespace paint::render {

class CanvasRenderer {
   public:
    void render(RenderContext& context, const ui::CanvasElement& canvas);

   private:
    void drawLineGrid(RenderContext& context, int width, int height, int spacing = 25) const;
    void drawDotGrid(RenderContext& context, int width, int height, int spacing = 25) const;

    SceneRenderer _sceneRenderer;
};

}  // namespace paint::render
