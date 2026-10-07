#include "render/renderer/color_field_renderer.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "common/color.hpp"
#include "ui/elements.hpp"

namespace paint::render {

void ColorFieldRenderer::render(RenderContext& context, const ui::ColorFieldElement& field) {
    if (field.width() <= 0 || field.height() <= 0) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    renderPanel(context, field);
    renderMarker(context, field);
    glPopAttrib();
}

void ColorFieldRenderer::renderPanel(RenderContext&, const ui::ColorFieldElement& field) {
    const ColorRGBA hueColor = hsv2rgb({field.hue(), 1.0f, 1.0f});

    const float width = static_cast<float>(field.width());
    const float height = static_cast<float>(field.height());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 第一層：由左側白色漸變至右側目前色相的純色。
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, height);
    glColor3f(hueColor.r, hueColor.g, hueColor.b);
    glVertex2f(width, height);
    glVertex2f(width, 0.0f);
    glEnd();

    // 第二層：由上方透明漸變至下方黑色，形成明度軸。
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(width, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(width, height);
    glVertex2f(0.0f, height);
    glEnd();
    glDisable(GL_BLEND);
}

void ColorFieldRenderer::renderMarker(RenderContext& context, const ui::ColorFieldElement& field) {
    const int markerX = static_cast<int>(std::lround(field.saturation() * std::max(0, field.width() - 1)));
    const int markerY = static_cast<int>(std::lround((1.0f - field.value()) * std::max(0, field.height() - 1)));

    // 黑白雙環讓選取標記在任何深淺的背景上都保持清楚。
    // 黑環向內填到半徑 4，再由白環覆蓋半徑 3～4，兩環之間不會出現空洞。
    context.fillRing(markerX, markerY, 4, 6, Color::Black);
    context.fillRing(markerX, markerY, 3, 4, Color::White);
}

}  // namespace paint::render
