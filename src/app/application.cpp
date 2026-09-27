#include "app/application.hpp"

#include <GL/freeglut.h>

namespace paint {

Application::Application(int& argc, char** argv) {
    // 初始化 GLUT
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // 在視窗關閉時，繼續執行程式，而不是退出 GLUT 主循環
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

void Application::run() {
    // 進入 GLUT 主循環
    glutMainLoop();
}

}  // namespace paint
