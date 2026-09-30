#include "render/render_context.hpp"

#include <GL/freeglut.h>

namespace paint {

void RenderContext::pushTransform() {
    glPushMatrix();
}

void RenderContext::popTransform() {
    glPopMatrix();
}

void RenderContext::translate(float x, float y) {
    glTranslatef(x, y, 0.0f);
}

}  // namespace paint
