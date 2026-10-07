#pragma once

#include <vector>

#include "drawing/tools/canvas_tool.hpp"
#include "render/render_context.hpp"
#include "render/renderer/scene_renderer.hpp"

namespace paint::ui {
class CanvasElement;
}

namespace paint::render {

using paint::drawing::ToolOverlay;

class CanvasRenderer {
   public:
    void render(RenderContext& context, const ui::CanvasElement& canvas);

   private:
    void drawLineGrid(RenderContext& context, int width, int height, int spacing = 25) const;
    void drawDotGrid(RenderContext& context, int width, int height, int spacing = 25) const;
    void drawSelectionBounds(RenderContext& context,
                             const std::vector<ToolOverlay::SelectionBounds>& bounds) const;

    SceneRenderer _sceneRenderer;
};

}  // namespace paint::render
