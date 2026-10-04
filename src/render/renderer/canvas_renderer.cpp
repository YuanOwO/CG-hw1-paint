#include "render/renderer/canvas_renderer.hpp"

#include <GL/freeglut.h>

#include "ui/elements/canvas.hpp"

namespace paint::render {

void CanvasRenderer::drawLineGrid(RenderContext& context, int width, int height, int spacing) const {
    if (width <= 0 || height <= 0 || spacing <= 0) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glColor4f(0.9f, 0.9f, 0.9f, 1.0f);  // 淺灰色
    glLineWidth(1.0f);

    glBegin(GL_LINES);

    for (int x = spacing; x < width; x += spacing) {
        glVertex2f(static_cast<float>(x), 0.0f);
        glVertex2f(static_cast<float>(x), static_cast<float>(height));
    }

    for (int y = spacing; y < height; y += spacing) {
        glVertex2f(0.0f, static_cast<float>(y));
        glVertex2f(static_cast<float>(width), static_cast<float>(y));
    }

    glEnd();
    glPopAttrib();
}

void CanvasRenderer::drawDotGrid(RenderContext& context, int width, int height, int spacing) const {
    if (width <= 0 || height <= 0 || spacing <= 0) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT | GL_ENABLE_BIT);
    glColor4f(0.9f, 0.9f, 0.9f, 1.0f);  // 淺灰色
    glPointSize(2.0f);
    glDisable(GL_POINT_SMOOTH);

    glBegin(GL_POINTS);
    for (int x = spacing; x < width; x += spacing) {
        for (int y = spacing; y < height; y += spacing) {
            glVertex2f(static_cast<float>(x), static_cast<float>(y));
        }
    }
    glEnd();
    glPopAttrib();
}

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
        drawLineGrid(context, canvas.width(), canvas.height());
    } else if (canvas.gridMode() == ui::GridMode::Dots) {
        drawDotGrid(context, canvas.width(), canvas.height());
    }

    // 3. 畫 Scene
    for (const auto& shape : canvas.document().scene().objects()) {
        _sceneRenderer.draw(context, *shape);
    }

    // 4. 畫 draft / preview
    if (const auto* draft = canvas.draft()) {
        _sceneRenderer.draw(context, *draft);
    }
}

}  // namespace paint::render
