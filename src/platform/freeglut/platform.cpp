// FreeGLUT 實作（Windows / Linux），macOS 見 apple/platform.cpp。

#ifndef __APPLE__

#  include <GL/freeglut.h>

#  include "platform/platform_internal.hpp"

namespace paint::platform {

static_assert(KEY_SHIFT_L == GLUT_KEY_SHIFT_L && KEY_SHIFT_R == GLUT_KEY_SHIFT_R);
static_assert(KEY_CTRL_L == GLUT_KEY_CTRL_L && KEY_CTRL_R == GLUT_KEY_CTRL_R);
static_assert(KEY_ALT_L == GLUT_KEY_ALT_L && KEY_ALT_R == GLUT_KEY_ALT_R);
static_assert(KEY_SUPER_L == GLUT_KEY_SUPER_L && KEY_SUPER_R == GLUT_KEY_SUPER_R);

namespace {

// 使用者關閉視窗。回傳後 FreeGLUT 會自行銷毀視窗
void closeCallback() {
    notifyClose(glutGetWindow());
}

}  // namespace

void initialize(int& argc, char** argv) {
    glutInit(&argc, argv);

    // 在視窗關閉時，繼續執行程式，而不是退出 GLUT 主循環
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

void runMainLoop() {
    // GLUT_ACTION_CONTINUE_EXECUTION 下，所有視窗關閉後返回
    glutMainLoop();
}

bool isRunning() {
    return glutGet(GLUT_INIT_STATE) != 0;
}

int createWindow(const char* title, WindowHandler& handler) {
    const int window = glutCreateWindow(title);

    registerWindow(window, handler);  // 共用的部分：查表與轉呼叫
    glutCloseFunc(closeCallback);     // FreeGLUT 專屬：關閉事件

    return window;
}

void destroyWindow(int window) {
    unregisterWindow(window);
    glutDestroyWindow(window);
}

}  // namespace paint::platform

#endif
