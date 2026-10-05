#include "render/renderer/hue_slider_renderer.hpp"

#include <GL/freeglut.h>

#include <algorithm>

#include "common/color.hpp"
#include "ui/elements/hue_slider.hpp"

namespace paint::render {

void HueSliderRenderer::render(RenderContext&, const ui::HueSliderElement& slider) {
    if (slider.width() <= 0 || slider.height() <= 0) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 色環的六個主要區段可由 OpenGL 頂點色彩插值成連續漸層。
    constexpr int SEGMENTS = 6;
    glBegin(GL_QUADS);
    for (int segment = 0; segment < SEGMENTS; ++segment) {
        const float y0 = static_cast<float>(slider.height()) * segment / SEGMENTS;
        const float y1 = static_cast<float>(slider.height()) * (segment + 1) / SEGMENTS;
        const ColorRGBA top = hsv2rgb({segment * 60.0f, 1.0f, 1.0f});
        const ColorRGBA bottom = hsv2rgb({(segment + 1) * 60.0f, 1.0f, 1.0f});

        glColor3f(top.r, top.g, top.b);
        glVertex2f(0.0f, y0);
        glVertex2f(static_cast<float>(slider.width()), y0);
        glColor3f(bottom.r, bottom.g, bottom.b);
        glVertex2f(static_cast<float>(slider.width()), y1);
        glVertex2f(0.0f, y1);
    }
    glEnd();

    // 黑色外框搭配中央白線，確保指示器在所有色相上都可辨識。
    const float markerY = slider.hue() / 360.0f * static_cast<float>(std::max(0, slider.height() - 1));
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-1.0f, markerY - 2.0f);
    glVertex2f(static_cast<float>(slider.width()) + 1.0f, markerY - 2.0f);
    glVertex2f(static_cast<float>(slider.width()) + 1.0f, markerY + 2.0f);
    glVertex2f(-1.0f, markerY + 2.0f);
    glEnd();

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, markerY);
    glVertex2f(static_cast<float>(slider.width()), markerY);
    glEnd();

    glPopAttrib();
}

}  // namespace paint::render
