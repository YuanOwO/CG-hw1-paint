#include "render/render_context.hpp"

#include <GL/freeglut.h>

namespace paint::render {

void RenderContext::pushTransform() {
    glPushMatrix();
}

void RenderContext::popTransform() {
    glPopMatrix();
}

void RenderContext::translate(float x, float y) {
    glTranslatef(x, y, 0.0f);
}

void RenderContext::fillRect(int width, int height, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glRecti(0, 0, width, height);
    glPopAttrib();
}

}  // namespace paint::render
