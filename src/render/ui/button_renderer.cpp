#include "render/ui/button_renderer.hpp"

#include <GL/freeglut.h>

#include "ui/elements/button.hpp"

namespace paint {

void ButtonRenderer::render(RenderContext& context, const ui::ButtonElement& button) {
    // 設置顏色
    if (button.isPressed()) {
        context.fillRect(button.width(), button.height(), Color::DarkGray);
    } else if (button.isHovered()) {
        context.fillRect(button.width(), button.height(), Color::LightGray);
    } else {
        context.fillRect(button.width(), button.height(), Color::Gray);
    }

    // 畫邊框
    glColor3f(0.0f, 0.0f, 0.0f);  // 黑色
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(static_cast<float>(button.width()), 0.0f);
    glVertex2f(static_cast<float>(button.width()), static_cast<float>(button.height()));
    glVertex2f(0.0f, static_cast<float>(button.height()));
    glEnd();
}

}  // namespace paint
