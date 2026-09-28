#include "app/window.hpp"

#include <GL/freeglut.h>

#include <unordered_map>

namespace paint {

namespace {
std::unordered_map<int, Window*> windows;

Key mapCharacter(int glutKey) {
    switch (glutKey) {
    // Letters
    case 'a':
    case 'A':
        return Key::A;
    case 'b':
    case 'B':
        return Key::B;
    case 'c':
    case 'C':
        return Key::C;
    case 'd':
    case 'D':
        return Key::D;
    case 'e':
    case 'E':
        return Key::E;
    case 'f':
    case 'F':
        return Key::F;
    case 'g':
    case 'G':
        return Key::G;
    case 'h':
    case 'H':
        return Key::H;
    case 'i':
    case 'I':
        return Key::I;
    case 'j':
    case 'J':
        return Key::J;
    case 'k':
    case 'K':
        return Key::K;
    case 'l':
    case 'L':
        return Key::L;
    case 'm':
    case 'M':
        return Key::M;
    case 'n':
    case 'N':
        return Key::N;
    case 'o':
    case 'O':
        return Key::O;
    case 'p':
    case 'P':
        return Key::P;
    case 'q':
    case 'Q':
        return Key::Q;
    case 'r':
    case 'R':
        return Key::R;
    case 's':
    case 'S':
        return Key::S;
    case 't':
    case 'T':
        return Key::T;
    case 'u':
    case 'U':
        return Key::U;
    case 'v':
    case 'V':
        return Key::V;
    case 'w':
    case 'W':
        return Key::W;
    case 'x':
    case 'X':
        return Key::X;
    case 'y':
    case 'Y':
        return Key::Y;
    case 'z':
    case 'Z':
        return Key::Z;

        // Number row
    case '0':
    case ')':
        return Key::Digit0;
    case '1':
    case '!':
        return Key::Digit1;
    case '2':
    case '@':
        return Key::Digit2;
    case '3':
    case '#':
        return Key::Digit3;
    case '4':
    case '$':
        return Key::Digit4;
    case '5':
    case '%':
        return Key::Digit5;
    case '6':
    case '^':
        return Key::Digit6;
    case '7':
    case '&':
        return Key::Digit7;
    case '8':
    case '*':
        return Key::Digit8;
    case '9':
    case '(':
        return Key::Digit9;

        // Symbols
    case '-':
    case '_':
        return Key::Minus;
    case '=':
    case '+':
        return Key::Equal;
    case '[':
    case '{':
        return Key::LeftBracket;
    case ']':
    case '}':
        return Key::RightBracket;
    case '\\':
    case '|':
        return Key::Backslash;
    case ';':
    case ':':
        return Key::Semicolon;
    case '\'':
    case '"':
        return Key::Apostrophe;
    case ',':
    case '<':
        return Key::Comma;
    case '.':
    case '>':
        return Key::Period;
    case '/':
    case '?':
        return Key::Slash;
    case '`':
    case '~':
        return Key::GraveAccent;

    // Common keys
    case ' ':
        return Key::Space;
    case 27:  // ESC
        return Key::Escape;
    case '\r':
    case '\n':
        return Key::Enter;
    case '\b':
        return Key::Backspace;
    case '\t':
        return Key::Tab;

    // Delete
    case 127:
        return Key::Delete;

    // Unhandled keys
    default:
        return Key::Unknown;
    }
}

Key mapSpecialKey(int glutKey) {
    switch (glutKey) {
    // Navigation
    case GLUT_KEY_LEFT:
        return Key::Left;
    case GLUT_KEY_RIGHT:
        return Key::Right;
    case GLUT_KEY_UP:
        return Key::Up;
    case GLUT_KEY_DOWN:
        return Key::Down;

    case GLUT_KEY_PAGE_UP:
        return Key::PageUp;
    case GLUT_KEY_PAGE_DOWN:
        return Key::PageDown;
    case GLUT_KEY_HOME:
        return Key::Home;
    case GLUT_KEY_END:
        return Key::End;
    case GLUT_KEY_INSERT:
        return Key::Insert;

    // Function keys
    case GLUT_KEY_F1:
        return Key::F1;
    case GLUT_KEY_F2:
        return Key::F2;
    case GLUT_KEY_F3:
        return Key::F3;
    case GLUT_KEY_F4:
        return Key::F4;
    case GLUT_KEY_F5:
        return Key::F5;
    case GLUT_KEY_F6:
        return Key::F6;
    case GLUT_KEY_F7:
        return Key::F7;
    case GLUT_KEY_F8:
        return Key::F8;
    case GLUT_KEY_F9:
        return Key::F9;
    case GLUT_KEY_F10:
        return Key::F10;
    case GLUT_KEY_F11:
        return Key::F11;
    case GLUT_KEY_F12:
        return Key::F12;

    // Modifiers
    case GLUT_KEY_SHIFT_L:
        return Key::LeftShift;
    case GLUT_KEY_SHIFT_R:
        return Key::RightShift;
    case GLUT_KEY_CTRL_L:
        return Key::LeftCtrl;
    case GLUT_KEY_CTRL_R:
        return Key::RightCtrl;
    case GLUT_KEY_ALT_L:
        return Key::LeftAlt;
    case GLUT_KEY_ALT_R:
        return Key::RightAlt;
    case GLUT_KEY_SUPER_L:
        return Key::LeftSuper;
    case GLUT_KEY_SUPER_R:
        return Key::RightSuper;

    // Unhandled keys
    default:
        return Key::Unknown;
    }
}

MouseButton mapMouseButton(int glutButton) {
    switch (glutButton) {
    case GLUT_LEFT_BUTTON:
        return MouseButton::MouseLeft;
    case GLUT_MIDDLE_BUTTON:
        return MouseButton::MouseMiddle;
    case GLUT_RIGHT_BUTTON:
        return MouseButton::MouseRight;

    // Unhandled buttons
    default:
        return MouseButton::Unknown;
    }
}

}  // namespace

KeyboardState Window::_keyboardState;  // 全局的鍵盤狀態

Window::Window(const std::string& title, int width, int height, bool resizable)
    : _title(title), _width(width), _height(height), _resizable(resizable) {
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

    // 啟動定時器，確保持續更新視窗內容
    glutTimerFunc(CAPTURE_INTERVAL_MS, Window::timerCallback, _id);  // 16ms 對應約 60 FPS
}

Window::~Window() {
    if (_id != 0) {
        windows.erase(_id);
        glutDestroyWindow(_id);
    }
}

void Window::setRootElement(std::unique_ptr<Element> rootElement) {
    _rootElement = std::move(rootElement);
    if (_rootElement) {
        // 設定根元素的 invalidate callback，當元素需要重新渲染時，呼叫此函式通知父視窗
        _rootElement->setInvalidateCallback([this]() {
            // 標記視窗內容為 dirty，並請求重新渲染
            _contentDirty = true;
            _needsCapture = false;
            this->requestRedisplay();
        });
    }
    requestRedisplay();
}

void Window::requestRedisplay() {
    // 如果視窗已經關閉，直接返回
    if (_id == 0) {
        return;
    }

    glutPostWindowRedisplay(_id);
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

#pragma region GLUT timer callback

void Window::timerCallback(int windowId) {
    auto it = windows.find(windowId);
    if (it == windows.end()) {
        return;  // 目標視窗已關閉，不再續約
    }

    auto* window = it->second;

    // 如果視窗需要捕捉內容到 ColorBuffer，且內容沒有被標記為 dirty，則進行捕捉
    if (window->_needsCapture && !window->_contentDirty) {
        const int previousWindow = glutGetWindow();

        // Timer 不會幫你選視窗，讀取 framebuffer 前要自行切換。
        glutSetWindow(windowId);

        window->_colorBuffer.capture(window->_width, window->_height);
        window->_needsCapture = false;

        if (previousWindow != 0) {
            glutSetWindow(previousWindow);
        }
    }

    // 重新啟動 timer，確保持續更新
    glutTimerFunc(CAPTURE_INTERVAL_MS, Window::timerCallback, windowId);
}

#pragma endregion  // GLUT timer callback

#pragma region GLUT window callbacks

void Window::closeCallback() {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    // 從管理列表中移除視窗，並將其 ID 設為 0
    windows.erase(window->_id);
    window->_id = 0;

    const WindowCloseEvent event;

    window->onClose(event);
}

void Window::reshapeCallback(int width, int height) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    if (!window->isResizable() && (width != window->_width || height != window->_height)) {
        // 如果視窗不可調整大小，則恢復到原始大小
        glutReshapeWindow(window->_width, window->_height);
        return;
    }

    window->_width = width;
    window->_height = height;

    window->_colorBuffer.resize(width, height);  // 調整 ColorBuffer 的大小
    window->_contentDirty = true;
    window->_needsCapture = false;

    // 零尺寸 viewport 合法，表示沒有可繪製的區域。
    glViewport(0, 0, width, height);

    // glOrtho 的左右、上下界不能相等。
    if (width > 0 && height > 0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, static_cast<GLdouble>(width), static_cast<GLdouble>(height), 0.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    const WindowResizeEvent event(width, height);

    window->onResize(event);

    window->requestRedisplay();
}

void Window::visibilityCallback(int state) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    using State = WindowVisibilityEvent::WindowVisibilityState;

    const WindowVisibilityEvent event(state == GLUT_VISIBLE ? State::Visible : State::Hidden);

    if (state == GLUT_VISIBLE) {
        window->requestRedisplay();
    } else {
        window->_contentDirty = true;
        window->_needsCapture = false;
    }

    window->onVisibilityChange(event);
}

void Window::displayCallback() {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    // 清除顯示緩衝區
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 設置背景色為白色
    glClear(GL_COLOR_BUFFER_BIT);

    // 如果視窗內容被標記為 dirty，或者需要捕捉內容到 ColorBuffer，
    // 或者 ColorBuffer 的大小與視窗不匹配，則重新渲染視窗內容
    if (window->_contentDirty || window->_needsCapture ||
        !window->_colorBuffer.matchesSize(window->_width, window->_height)) {
        window->renderContent();

        window->_contentDirty = false;
        window->_needsCapture = true;
    } else {
        // 直接從 ColorBuffer 恢復視窗內容，避免不必要的重繪
        window->_colorBuffer.restore();
    }

    // 提交繪圖命令
    glFlush();
}

#pragma endregion  // GLUT window callbacks

#pragma region GLUT keyboard callbacks

void Window::keyDownHandler(Key key, int x, int y) {
    auto* window = getCurrentWindow();

    bool firstPress = _keyboardState._press(key);

    // 找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    window->_mouseState._setMousePosition(x, y);

    const KeyboardEvent event(key, ButtonAction::Down, _keyboardState, window->_mouseState, !firstPress);

    window->onKeyDown(event);
}

void Window::keyUpHandler(Key key, int x, int y) {
    auto* window = getCurrentWindow();

    _keyboardState._release(key);

    // 找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    window->_mouseState._setMousePosition(x, y);

    const KeyboardEvent event(key, ButtonAction::Up, _keyboardState, window->_mouseState);

    window->onKeyUp(event);
}

void Window::keyboardCallback(unsigned char key, int x, int y) {
    auto btn = mapCharacter(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyDownHandler(btn, x, y);
}

void Window::keyboardUpCallback(unsigned char key, int x, int y) {
    auto btn = mapCharacter(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyUpHandler(btn, x, y);
}

void Window::specialCallback(int key, int x, int y) {
    auto btn = mapSpecialKey(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyDownHandler(btn, x, y);
}

void Window::specialUpCallback(int key, int x, int y) {
    auto btn = mapSpecialKey(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyUpHandler(btn, x, y);
}

#pragma endregion  // GLUT keyboard callbacks

#pragma region GLUT mouse callbacks

void Window::mouseCallback(int button, int state, int x, int y) {
    auto* window = getCurrentWindow();

    auto btn = mapMouseButton(button);

    // 如果找不到當前視窗或未知按鈕，直接返回
    if (!window || btn == MouseButton::Unknown) {
        return;
    }

    window->_mouseState._setMousePosition(x, y);

    if (state == GLUT_DOWN) {
        window->_mouseState._press(btn);

        // 記錄滑鼠按下的位置，方便後續判斷點擊事件
        auto& press = window->_clickCandidate[btn];
        press.active = true;
        press.position = Point(x, y);

        const MouseEvent event(btn, ButtonAction::Down, _keyboardState, window->_mouseState);
        window->onMouseDown(event);
    } else if (state == GLUT_UP) {
        window->_mouseState._release(btn);

        const MouseEvent event(btn, ButtonAction::Up, _keyboardState, window->_mouseState);
        window->onMouseUp(event);

        // 判斷是否為點擊事件
        // 如果滑鼠按下和釋放的位置距離小於閾值，則認為是點擊事件
        auto& press = window->_clickCandidate[btn];
        if (press.active && abs(window->_mouseState.getPosition() - press.position) <= CLICK_MOVE_THRESHOLD) {
            // 判斷是否為雙擊事件
            // 1. 上一次點擊事件有效
            // 2. 距離現在的時間小於閾值
            // 3. 上一次點擊事件的位置與現在的位置距離小於閾值

            auto& lastClick = window->_lastClicks[btn];
            auto now = std::chrono::steady_clock::now();
            const Point position = window->_mouseState.getPosition();

            const bool isDoubleClick = lastClick.active &&
                                       now - lastClick.time <= DOUBLE_CLICK_TIME_THRESHOLD &&
                                       abs(position - lastClick.position) <= CLICK_MOVE_THRESHOLD;

            if (isDoubleClick) {
                lastClick.active = false;  // 重置上一次點擊事件，避免三擊事件被誤判為雙擊事件

                const MouseClickEvent doubleClickEvent(btn, 2, _keyboardState, window->_mouseState);
                window->onDoubleClick(doubleClickEvent);
            } else {
                lastClick.active = true;
                lastClick.position = window->_mouseState.getPosition();
                lastClick.time = now;

                const MouseClickEvent clickEvent(btn, 1, _keyboardState, window->_mouseState);
                window->onClick(clickEvent);
            }
        }

        // 重置滑鼠按下狀態
        press.active = false;
    } else {
        // 未知狀態，直接返回
        return;
    }
}

void Window::mouseMoveHandler(int x, int y) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    window->_mouseState._setMousePosition(x, y);

    // 移動距離超過閾值，則取消所有滑鼠按下狀態，避免誤判為點擊事件
    for (auto& [button, press] : window->_clickCandidate) {
        if (press.active && abs(window->_mouseState.getPosition() - press.position) > CLICK_MOVE_THRESHOLD) {
            press.active = false;
        }
    }

    const MouseMoveEvent event(_keyboardState, window->_mouseState);

    window->onMouseMove(event);
}

void Window::motionCallback(int x, int y) {
    mouseMoveHandler(x, y);
}

void Window::passiveMotionCallback(int x, int y) {
    mouseMoveHandler(x, y);
}

void Window::entryCallback(int state) {
    auto* window = getCurrentWindow();

    // 如果找不到當前視窗，直接返回
    if (!window) {
        return;
    }

    // 清除滑鼠按鈕狀態，避免在滑鼠進入或離開視窗時，按鈕狀態不一致。
    window->_mouseState._clear();

    if (state == GLUT_ENTERED) {
        MouseEnterEvent event(_keyboardState, window->_mouseState);
        window->onMouseEnter(event);
    } else if (state == GLUT_LEFT) {
        MouseLeaveEvent event(_keyboardState, window->_mouseState);
        window->onMouseLeave(event);
    }
}

#pragma endregion  // GLUT mouse callbacks

}  // namespace paint
