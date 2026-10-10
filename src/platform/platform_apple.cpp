// Apple GLUT 實作（macOS），Windows / Linux 見 platform_freeglut.cpp。
//
// 與 FreeGLUT 的差異：
//   - 關閉視窗時只呼叫 callback，不會銷毀視窗 → 呼叫後自行銷毀
//   - glutMainLoop 不會返回 → 以 glutCheckLoop 自行執行主循環，所有視窗關閉後返回

#ifdef __APPLE__

#include <GLUT/glut.h>

#include <unordered_set>

#include "platform/platform_internal.hpp"

namespace paint::platform {

namespace {

// 存在中的視窗，全部關閉後 runMainLoop 返回
std::unordered_set<int> windows;

bool running = true;

// 使用者關閉視窗。Apple GLUT 只呼叫這個 callback，不會銷毀視窗
void closeCallback() {
    const int window = glutGetWindow();

    notifyClose(window);
    destroyWindow(window);
}

}  // namespace

void initialize(int& argc, char** argv) {
    // 關閉視窗時不結束程式：每個視窗都註冊了 glutWMCloseFunc（見 createWindow）
    glutInit(&argc, argv);
}

void runMainLoop() {
    // Apple GLUT 的 glutMainLoop 不會返回，改為每次處理一輪事件，直到所有視窗關閉
    while (!windows.empty()) {
        glutCheckLoop();
    }

    running = false;
}

bool isRunning() {
    return running;
}

int createWindow(const char* title, WindowHandler& handler) {
    const int window = glutCreateWindow(title);
    windows.insert(window);

    registerWindow(window, handler);  // 共用的部分：查表與轉呼叫

    // 未註冊時，Apple GLUT 關閉任何視窗都會結束程式
    glutWMCloseFunc(closeCallback);

    return window;
}

void destroyWindow(int window) {
    // 已經銷毀過（例如使用者關閉視窗後，又由程式銷毀）
    if (windows.erase(window) == 0) {
        return;
    }

    unregisterWindow(window);
    glutDestroyWindow(window);
}

}  // namespace paint::platform

#endif
