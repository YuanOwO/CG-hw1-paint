#include "window/window.hpp"

#include <GL/freeglut.h>

#include <iostream>
#include <unordered_map>

namespace paint {

namespace {
std::unordered_map<int, Window*> windows;

Button mapCharacter(int glutKey) {
    switch (glutKey) {
    // Letters
    case 'a':
    case 'A':
        return Button::A;
    case 'b':
    case 'B':
        return Button::B;
    case 'c':
    case 'C':
        return Button::C;
    case 'd':
    case 'D':
        return Button::D;
    case 'e':
    case 'E':
        return Button::E;
    case 'f':
    case 'F':
        return Button::F;
    case 'g':
    case 'G':
        return Button::G;
    case 'h':
    case 'H':
        return Button::H;
    case 'i':
    case 'I':
        return Button::I;
    case 'j':
    case 'J':
        return Button::J;
    case 'k':
    case 'K':
        return Button::K;
    case 'l':
    case 'L':
        return Button::L;
    case 'm':
    case 'M':
        return Button::M;
    case 'n':
    case 'N':
        return Button::N;
    case 'o':
    case 'O':
        return Button::O;
    case 'p':
    case 'P':
        return Button::P;
    case 'q':
    case 'Q':
        return Button::Q;
    case 'r':
    case 'R':
        return Button::R;
    case 's':
    case 'S':
        return Button::S;
    case 't':
    case 'T':
        return Button::T;
    case 'u':
    case 'U':
        return Button::U;
    case 'v':
    case 'V':
        return Button::V;
    case 'w':
    case 'W':
        return Button::W;
    case 'x':
    case 'X':
        return Button::X;
    case 'y':
    case 'Y':
        return Button::Y;
    case 'z':
    case 'Z':
        return Button::Z;

        // Number row
    case '0':
    case ')':
        return Button::Digit0;
    case '1':
    case '!':
        return Button::Digit1;
    case '2':
    case '@':
        return Button::Digit2;
    case '3':
    case '#':
        return Button::Digit3;
    case '4':
    case '$':
        return Button::Digit4;
    case '5':
    case '%':
        return Button::Digit5;
    case '6':
    case '^':
        return Button::Digit6;
    case '7':
    case '&':
        return Button::Digit7;
    case '8':
    case '*':
        return Button::Digit8;
    case '9':
    case '(':
        return Button::Digit9;

        // Symbols
    case '-':
    case '_':
        return Button::Minus;
    case '=':
    case '+':
        return Button::Equal;
    case '[':
    case '{':
        return Button::LeftBracket;
    case ']':
    case '}':
        return Button::RightBracket;
    case '\\':
    case '|':
        return Button::Backslash;
    case ';':
    case ':':
        return Button::Semicolon;
    case '\'':
    case '"':
        return Button::Apostrophe;
    case ',':
    case '<':
        return Button::Comma;
    case '.':
    case '>':
        return Button::Period;
    case '/':
    case '?':
        return Button::Slash;
    case '`':
    case '~':
        return Button::GraveAccent;

    // Common keys
    case ' ':
        return Button::Space;
    case 27:  // ESC
        return Button::Escape;
    case '\r':
    case '\n':
        return Button::Enter;
    case '\b':
        return Button::Backspace;
    case '\t':
        return Button::Tab;

    // Delete
    case 127:
        return Button::Delete;

    // Unhandled keys
    default:
        return Button::Unknown;
    }
}

Button mapSpecialKey(int glutKey) {
    switch (glutKey) {
    // Navigation
    case GLUT_KEY_LEFT:
        return Button::Left;
    case GLUT_KEY_RIGHT:
        return Button::Right;
    case GLUT_KEY_UP:
        return Button::Up;
    case GLUT_KEY_DOWN:
        return Button::Down;

    case GLUT_KEY_PAGE_UP:
        return Button::PageUp;
    case GLUT_KEY_PAGE_DOWN:
        return Button::PageDown;
    case GLUT_KEY_HOME:
        return Button::Home;
    case GLUT_KEY_END:
        return Button::End;
    case GLUT_KEY_INSERT:
        return Button::Insert;

    // Function keys
    case GLUT_KEY_F1:
        return Button::F1;
    case GLUT_KEY_F2:
        return Button::F2;
    case GLUT_KEY_F3:
        return Button::F3;
    case GLUT_KEY_F4:
        return Button::F4;
    case GLUT_KEY_F5:
        return Button::F5;
    case GLUT_KEY_F6:
        return Button::F6;
    case GLUT_KEY_F7:
        return Button::F7;
    case GLUT_KEY_F8:
        return Button::F8;
    case GLUT_KEY_F9:
        return Button::F9;
    case GLUT_KEY_F10:
        return Button::F10;
    case GLUT_KEY_F11:
        return Button::F11;
    case GLUT_KEY_F12:
        return Button::F12;

    // Modifiers
    case GLUT_KEY_SHIFT_L:
        return Button::LeftShift;
    case GLUT_KEY_SHIFT_R:
        return Button::RightShift;
    case GLUT_KEY_CTRL_L:
        return Button::LeftCtrl;
    case GLUT_KEY_CTRL_R:
        return Button::RightCtrl;
    case GLUT_KEY_ALT_L:
        return Button::LeftAlt;
    case GLUT_KEY_ALT_R:
        return Button::RightAlt;
    case GLUT_KEY_SUPER_L:
        return Button::LeftSuper;
    case GLUT_KEY_SUPER_R:
        return Button::RightSuper;

    // Unhandled keys
    default:
        return Button::Unknown;
    }
}

Button mapMouseButton(int glutButton) {
    switch (glutButton) {
    case GLUT_LEFT_BUTTON:
        return Button::MouseLeft;
    case GLUT_MIDDLE_BUTTON:
        return Button::MouseMiddle;
    case GLUT_RIGHT_BUTTON:
        return Button::MouseRight;

    // Unhandled buttons
    default:
        return Button::Unknown;
    }
}

}  // namespace

InputState Window::_inputState{};

Window::Window(const std::string& title, int width, int height)
    : _title(title), _width(width), _height(height) {
    // 創建 GLUT 視窗
    glutInitWindowSize(_width, _height);
    _id = glutCreateWindow(_title.c_str());

    // 註冊 GLUT 回調函數
    glutCloseFunc(Window::closeCallback);

    glutReshapeFunc(Window::reshapeCallback);
    glutVisibilityFunc(Window::visibilityCallback);
    glutDisplayFunc(Window::displayCallback);

    glutKeyboardFunc(Window::keyboardCallback);
    glutKeyboardUpFunc(Window::keyboardUpCallback);
    glutSpecialFunc(Window::specialCallback);
    glutSpecialUpFunc(Window::specialUpCallback);

    glutMouseFunc(Window::mouseCallback);
    glutMotionFunc(Window::motionCallback);
    glutPassiveMotionFunc(Window::passiveMotionCallback);
    glutEntryFunc(Window::entryCallback);

    // 將視窗加入管理列表
    windows[_id] = this;
}

Window::~Window() {
    if (_id != 0) {
        windows.erase(_id);
        glutDestroyWindow(_id);
    }
}

Window* Window::getCurrentWindow() {
    const int currentWindowId = glutGetWindow();

    auto it = windows.find(currentWindowId);
    if (it != windows.end()) {
        return it->second;
    }

    return nullptr;
}

// --------------------------------------------------
// GLUT callbacks
// --------------------------------------------------

#pragma region GLUT Window callbacks

void Window::closeCallback() {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    // 從管理列表中移除視窗，並將其 ID 設為 0
    windows.erase(window->_id);
    window->_id = 0;

    const WindowCloseEvent event;

    window->onClose(event);
}

void Window::reshapeCallback(int width, int height) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    window->_width = width;
    window->_height = height;

    const WindowResizeEvent event(width, height);

    window->onResize(event);
}

void Window::visibilityCallback(int state) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    // TODO: 可以在這裡處理視窗可見性變化的事件
}

void Window::displayCallback() {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    // TODO: 可以在這裡處理視窗重繪的事件
}

#pragma endregion  // GLUT Window callbacks

#pragma region GLUT HID callbacks

void Window::keyboardCallback(unsigned char key, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapCharacter(key);

    _inputState._setMousePosition(x, y);
    bool firstPress = _inputState._press(btn);

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == Button::Unknown) {
        return;
    }

    const KeyboardEvent event(btn, ButtonAction::Down, _inputState, !firstPress);

    window->onKeyDown(event);
}

void Window::keyboardUpCallback(unsigned char key, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapCharacter(key);

    _inputState._setMousePosition(x, y);
    _inputState._release(btn);

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == Button::Unknown) {
        return;
    }

    const KeyboardEvent event(btn, ButtonAction::Up, _inputState);

    window->onKeyUp(event);
}

void Window::specialCallback(int key, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapSpecialKey(key);

    _inputState._setMousePosition(x, y);
    bool firstPress = _inputState._press(btn);

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == Button::Unknown) {
        return;
    }

    const KeyboardEvent event(btn, ButtonAction::Down, _inputState, !firstPress);

    window->onKeyDown(event);
}

void Window::specialUpCallback(int key, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapSpecialKey(key);

    _inputState._setMousePosition(x, y);
    _inputState._release(btn);

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == Button::Unknown) {
        return;
    }

    const KeyboardEvent event(btn, ButtonAction::Up, _inputState);

    window->onKeyUp(event);
}

void Window::mouseCallback(int button, int state, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapMouseButton(button);

    _inputState._setMousePosition(x, y);
    if (state == GLUT_DOWN) {
        _inputState._press(btn);
    } else if (state == GLUT_UP) {
        _inputState._release(btn);
    }

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == Button::Unknown) {
        return;  // 未知按鈕，直接返回
    }

    const MouseEvent event(btn, state == GLUT_DOWN ? ButtonAction::Down : ButtonAction::Up, _inputState);

    if (state == GLUT_DOWN) {
        window->onMouseDown(event);
    } else if (state == GLUT_UP) {
        window->onMouseUp(event);
    }
}

void Window::motionCallback(int x, int y) {
    auto* window = getCurrentWindow();

    _inputState._setMousePosition(x, y);

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    const MouseMoveEvent event(x, y, _inputState);

    window->onMouseMove(event);
}

void Window::passiveMotionCallback(int x, int y) {
    auto* window = getCurrentWindow();

    _inputState._setMousePosition(x, y);

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    const MouseMoveEvent event(x, y, _inputState);

    window->onMouseMove(event);
}

void Window::entryCallback(int state) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) return;

    // TODO: 可以在這裡處理滑鼠進入或離開視窗的事件
}

#pragma endregion  // GLUT HID callbacks

}  // namespace paint
