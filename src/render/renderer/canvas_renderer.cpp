#include "render/renderer/canvas_renderer.hpp"

#include "ui/elements/canvas.hpp"

namespace paint::render {

void CanvasRenderer::render(RenderContext& context, const ui::CanvasElement& canvas) {
    // 1. 畫背景
    // context.setColor(canvas.backgroundColor());
    // context.fillRect({
    //     0,
    //     0,
    //     canvas.width(),
    //     canvas.height()
    // });

    // 2. 畫格線
    if (canvas.gridMode() == ui::GridMode::Lines) {
        _shapeRenderer.drawLineGrid(context, canvas.width(), canvas.height());
    } else if (canvas.gridMode() == ui::GridMode::Dots) {
        _shapeRenderer.drawDotGrid(context, canvas.width(), canvas.height());
    }

    // 3. 畫 Scene
    for (const auto& shape : canvas.document().scene().shapes()) {
        _shapeRenderer.draw(context, *shape);
    }

    // 4. 畫 draft / preview
    if (const auto* draft = canvas.draft()) {
        _shapeRenderer.draw(context, *draft);
    }
}

}  // namespace paint::render
