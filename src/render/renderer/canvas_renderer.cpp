#include "render/renderer/canvas_renderer.hpp"

#include <memory>

#include "app/document.hpp"
#include "common/point.hpp"
#include "drawing/scene.hpp"
#include "platform/glut.hpp"
#include "ui/elements.hpp"

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

void CanvasRenderer::drawSelectionBounds(
    RenderContext& context, const std::vector<drawing::ToolOverlay::SelectionBounds>& selectionBounds) const {
    for (const auto& bounds : selectionBounds) {
        const float left = bounds.topLeft.x();
        const float top = bounds.topLeft.y();
        const float right = bounds.bottomRight.x();
        const float bottom = bounds.bottomRight.y();

        glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_ENABLE_BIT);
        glColor4f(0.1f, 0.45f, 1.0f, 1.0f);
        glLineWidth(1.0f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xF0F0);

        glBegin(GL_LINE_LOOP);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);
        glVertex2f(left, bottom);
        glEnd();

        glDisable(GL_LINE_STIPPLE);
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);
        glVertex2f(left, bottom);
        glEnd();
        glPopAttrib();
    }
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
    for (const auto& object : canvas.document().scene().objects()) {
        _sceneRenderer.draw(context, *object);
    }

    const auto overlay = canvas.toolOverlay();

    // 4. 畫工具預覽物件
    for (const auto* preview : overlay.previewObjects) {
        if (preview) {
            _sceneRenderer.draw(context, *preview);
        }
    }

    // 5. 畫選取框
    drawSelectionBounds(context, overlay.selectionBounds);
}

}  // namespace paint::render
