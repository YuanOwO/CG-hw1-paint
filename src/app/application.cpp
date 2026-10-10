#include "app/application.hpp"

#include <algorithm>
#include <stdexcept>

#include "platform/glut.hpp"

namespace paint::app {

namespace {

Application* currentApp = nullptr;

}

bool Application::_instanceExists = false;

Application::Application(int& argc, char** argv) {
    if (_instanceExists) {
        throw std::runtime_error("Only one instance of Application is allowed.");
    }

    _instanceExists = true;
    currentApp = this;

    // 初始化 GLUT
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutIdleFunc(idleCallback);

    // 在視窗關閉時，繼續執行程式，而不是退出 GLUT 主循環
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

Application::~Application() {
    _instanceExists = false;
    currentApp = nullptr;
}

Application& Application::current() {
    if (!currentApp) {
        throw std::runtime_error("No current Application instance.");
    }
    return *currentApp;
}

void Application::run() {
    // 進入 GLUT 主循環
    glutMainLoop();
}

void Application::removeClosedWindows() {
    _windows.erase(std::remove_if(_windows.begin(), _windows.end(),
                                  [](const auto& window) { return window->shouldClose(); }),
                   _windows.end());
}

void Application::idleCallback() {
    if (!_instanceExists) {
        return;  // 如果 Application 實例不存在，直接返回
    }

    currentApp->removeClosedWindows();
}

}  // namespace paint::app
