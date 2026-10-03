#include "render/ui/button_renderer.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "ui/elements/button.hpp"

namespace paint {
namespace {

void roundedRect(float x, float y, float width, float height, float radius, const ColorRGBA& color) {
    if (width <= 0 || height <= 0) return;
    radius = std::min(radius, std::min(width, height) * 0.5f);
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_POLYGON);
    for (int corner = 0; corner < 4; ++corner) {
        const float cx = x + (corner == 0 || corner == 3 ? width - radius : radius);
        const float cy = y + (corner < 2 ? height - radius : radius);
        for (int step = 0; step <= 12; ++step) {
            const float angle = (corner + step / 12.0f) * 1.57079632679f;
            glVertex2f(cx + radius * std::cos(angle), cy + radius * std::sin(angle));
        }
    }
    glEnd();
}

}  // namespace

void ButtonRenderer::render(RenderContext&, const ui::ButtonElement& button) {
    const auto& style = button.style();
    const auto& fill = button.isPressed() ? style.pressed
                       : button.isHovered() ? style.hovered : style.background;

    glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    roundedRect(0, 0, button.width(), button.height(), 7, style.border);
    roundedRect(1, 1, button.width() - 2, button.height() - 2, 6, fill);
    glPopAttrib();
}

}  // namespace paint
