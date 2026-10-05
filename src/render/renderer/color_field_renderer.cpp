#include "render/renderer/color_field_renderer.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "common/color.hpp"
#include "ui/elements/color_field.hpp"

namespace paint::render {

void ColorFieldRenderer::render(RenderContext&, const ui::ColorFieldElement& field) {
    if (field.width() <= 0 || field.height() <= 0) {
        return;
    }

    const ColorRGBA hueColor = hsv2rgb({field.hue(), 1.0f, 1.0f});

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 第一層：由左側白色漸變至右側目前色相的純色。
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, static_cast<float>(field.height()));
    glColor3f(hueColor.r, hueColor.g, hueColor.b);
    glVertex2f(static_cast<float>(field.width()), static_cast<float>(field.height()));
    glVertex2f(static_cast<float>(field.width()), 0.0f);
    glEnd();

    // 第二層：由上方透明漸變至下方黑色，形成明度軸。
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(static_cast<float>(field.width()), 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(static_cast<float>(field.width()), static_cast<float>(field.height()));
    glVertex2f(0.0f, static_cast<float>(field.height()));
    glEnd();

    const float markerX = field.saturation() * static_cast<float>(std::max(0, field.width() - 1));
    const float markerY = (1.0f - field.value()) * static_cast<float>(std::max(0, field.height() - 1));

    // 黑白雙環讓選取標記在任何深淺的背景上都保持清楚。
    glDisable(GL_BLEND);
    glLineWidth(2.0f);
    for (int ring = 0; ring < 2; ++ring) {
        const float radius = ring == 0 ? 6.0f : 4.0f;
        const float color = ring == 0 ? 0.0f : 1.0f;
        glColor3f(color, color, color);
        glBegin(GL_LINE_LOOP);
        for (int step = 0; step < 32; ++step) {
            const float angle = static_cast<float>(step) * 2.0f * PI / 32.0f;
            glVertex2f(markerX + std::cos(angle) * radius, markerY + std::sin(angle) * radius);
        }
        glEnd();
    }

    glPopAttrib();
}

}  // namespace paint::render
