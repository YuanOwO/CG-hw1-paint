#include "app/windows/graphics_render.hpp"

#include "platform/glut.hpp"

namespace paint::app {

render::ColorBuffer GraphicsRenderWindow::capture() {
    const int previousWindow = glutGetWindow();

    try {
        glutSetWindow(id());

        // 匯出視窗不一定經過 GLUT reshape callback，
        // 因此在同步渲染前自行設定 viewport 和投影矩陣。
        glViewport(0, 0, width(), height());

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, static_cast<GLdouble>(width()), static_cast<GLdouble>(height()), 0.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 呼叫 Window 原本的 UI element 渲染流程：
        // RootElement -> CanvasElement -> CanvasRenderer。
        ensureLayout();
        render();

        // 確保所有繪圖命令完成後再讀取 framebuffer。
        glFinish();

        render::ColorBuffer buffer;
        buffer.capture(0, 0, width(), height());

        if (previousWindow != 0 && previousWindow != id()) {
            glutSetWindow(previousWindow);
        }

        return buffer;
    } catch (...) {
        if (previousWindow != 0 && previousWindow != id()) {
            glutSetWindow(previousWindow);
        }

        throw;
    }
}

}  // namespace paint::app
