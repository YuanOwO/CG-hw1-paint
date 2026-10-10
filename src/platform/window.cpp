// 兩個平台共用的視窗事件分派：記住每個視窗的 handler，
// GLUT callback 發生時，找出對應的 handler 並呼叫它的成員函數。

#include <unordered_map>

#include "platform/glut.hpp"
#include "platform/platform_internal.hpp"

namespace paint::platform {

namespace {

// 視窗 ID → handler
std::unordered_map<int, WindowHandler*> handlers;

// GLUT 呼叫 callback 前，會將目前視窗設為事件發生的視窗
WindowHandler* currentHandler() {
    return findWindowHandler(glutGetWindow());
}

void reshapeCallback(int width, int height) {
    if (auto* handler = currentHandler()) {
        handler->onReshape(width, height);
    }
}

void visibilityCallback(int state) {
    if (auto* handler = currentHandler()) {
        handler->onVisibility(state);
    }
}

void displayCallback() {
    if (auto* handler = currentHandler()) {
        handler->onDisplay();
    }
}

void keyboardCallback(unsigned char key, int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onKeyboard(key, x, y);
    }
}

void keyboardUpCallback(unsigned char key, int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onKeyboardUp(key, x, y);
    }
}

void specialCallback(int key, int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onSpecial(key, x, y);
    }
}

void specialUpCallback(int key, int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onSpecialUp(key, x, y);
    }
}

void mouseCallback(int button, int state, int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onMouse(button, state, x, y);
    }
}

void motionCallback(int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onMotion(x, y);
    }
}

void passiveMotionCallback(int x, int y) {
    if (auto* handler = currentHandler()) {
        handler->onPassiveMotion(x, y);
    }
}

void entryCallback(int state) {
    if (auto* handler = currentHandler()) {
        handler->onEntry(state);
    }
}

}  // namespace

void registerWindow(int window, WindowHandler& handler) {
    handlers[window] = &handler;

    glutReshapeFunc(reshapeCallback);
    glutVisibilityFunc(visibilityCallback);
    glutDisplayFunc(displayCallback);

    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialCallback);
    glutSpecialUpFunc(specialUpCallback);

    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotionCallback);
    glutEntryFunc(entryCallback);
}

WindowHandler* findWindowHandler(int window) {
    auto it = handlers.find(window);
    return it != handlers.end() ? it->second : nullptr;
}

void unregisterWindow(int window) {
    handlers.erase(window);
}

void notifyClose(int window) {
    auto it = handlers.find(window);
    if (it == handlers.end()) {
        return;
    }

    // 先忘記 handler，關閉後不會再收到這個視窗的事件
    WindowHandler& handler = *it->second;
    handlers.erase(it);

    handler.onClose();
}

}  // namespace paint::platform
