#include "ui/window.hpp"

#include <utility>

#include "platform/glut.hpp"
#include "platform/platform.hpp"
#include "render/render_context.hpp"
#include "ui/elements.hpp"

namespace paint::ui {

namespace {

std::unordered_map<int, Window*> windows;
Window* modalWindow = nullptr;

}  // namespace

KeyboardState Window::_keyboardState{};  // 全局的鍵盤狀態

Window::Window(const std::string& title, int width, int height, bool resizable)
    : _title(title),
      _width(width),
      _height(height),
      _resizable(resizable),
      _rootElement(std::make_unique<RootElement>()) {
    // 創建 GLUT 視窗，之後這個視窗的事件都會呼叫 on* 成員函數
    glutInitWindowSize(_width, _height);
    _id = platform::createWindow(_title.c_str(), *this);

    // 將視窗加入管理列表
    windows[_id] = this;

    // 將根元素的 window 指針設置為當前視窗
    _rootElement->_window = this;

    // 啟動定時器，確保持續更新視窗內容
    glutTimerFunc(CAPTURE_INTERVAL_MS, timerCallback, _id);  // 16ms 對應約 60 FPS
}

Window::~Window() {
    if (modalWindow == this) {
        modalWindow = nullptr;
    }

    if (_id != 0) {
        windows.erase(_id);
        platform::destroyWindow(_id);
    }
}

void Window::setTitle(const std::string& title) {
    _title = title;

    if (_id != 0) {
        const int previousWindow = glutGetWindow();
        if (previousWindow != _id) {
            glutSetWindow(_id);
        }

        glutSetWindowTitle(_title.c_str());

        if (previousWindow != 0 && previousWindow != _id) {
            glutSetWindow(previousWindow);
        }
    }
}

void Window::resize(int width, int height) {
    if (_id == 0 || width <= 0 || height <= 0 || (width == _width && height == _height)) {
        return;
    }

    const int previousWindow = glutGetWindow();
    if (previousWindow != _id) {
        glutSetWindow(_id);
    }

    glutReshapeWindow(width, height);

    if (previousWindow != 0 && previousWindow != _id) {
        glutSetWindow(previousWindow);
    }
}

void Window::close() {
    _shouldClose = true;
}

void Window::setModal(bool modal) {
    if (modal) {
        modalWindow = this;

        // Clear input captured by another window before the dialog appeared.
        _keyboardState._clear();
        for (auto& [id, window] : windows) {
            window->resetInputState();
            window->_clickCandidate.clear();
            window->_lastClicks.clear();
            window->_mouseCapture = nullptr;
        }

        activateModalWindow();
    } else if (modalWindow == this) {
        modalWindow = nullptr;
    }
}

bool Window::canReceiveInput(int windowId) {
    return modalWindow == nullptr || modalWindow->id() == windowId;
}

void Window::activateModalWindow() {
    if (modalWindow == nullptr || modalWindow->id() == 0) {
        return;
    }

    glutSetWindow(modalWindow->id());
    glutShowWindow();
    glutPopWindow();
}

#pragma region Content

void Window::setContent(std::unique_ptr<Element> content) {
    _rootElement->setContent(std::move(content));

    requestLayout();
}

#pragma endregion  // Content

#pragma region Input state

void Window::resetInputState() {
    _keyboardState._clear();
    _mouseState._clear();
}

void Window::setFocusedElement(Element* element) {
    if (_focusedElement == element) {
        return;
    }

    // 在更改焦點前，先派送 Blur 事件給原本的焦點元素
    if (_focusedElement != nullptr) {
        BlurEvent event;
        _focusedElement->dispatchEvent(event);
    }

    _focusedElement = element;

    // 在更改焦點後，派送 Focus 事件給新的焦點元素
    if (_focusedElement != nullptr) {
        FocusEvent event;
        _focusedElement->dispatchEvent(event);
    }
}

#pragma endregion  // Input state

#pragma region Geometry

void Window::requestLayout() {
    _layoutDirty = true;
    requestRedisplay();  // 標記視窗需要重繪，因為佈局變化可能影響渲染
}

void Window::updateLayout() {
    _rootElement->measure({_width, _height});
    _rootElement->arrange({0, 0, _width, _height});

    _layoutDirty = false;
}

void Window::ensureLayout() {
    if (_layoutDirty) {
        updateLayout();
    }
}

#pragma endregion  // Geometry

#pragma region Rendering

void Window::render() {
    ensureLayout();  // 確保佈局是最新的

    render::RenderContext context;

    _rootElement->render(context);
}

void Window::requestRedisplay() {
    // 如果視窗已經關閉，直接返回
    if (_id == 0) {
        return;
    }

    _contentDirty = true;
    _needsCapture = false;

    requestCachedRedisplay();
}

void Window::requestCachedRedisplay() {
    if (_id == 0) {
        return;
    }

    glutPostWindowRedisplay(_id);
}

#pragma endregion  // Rendering

#pragma region Element interaction

Element* Window::hitTest(Point point) {
    ensureLayout();  // 確保佈局是最新的，才能正確命中測試

    return _rootElement->hitTest(point);
}

template <typename EventType, typename... Args>
Element* Window::dispatchMouseEvent(Args&&... args) {
    EventType event(std::forward<Args>(args)...);

    Element* target = _mouseCapture;

    if (target == nullptr) {
        target = hitTest(event.position());
    }

    // 決定事件的分派對象
    // 1. 如果命中了一個元素，則將事件分派給該元素
    // 2. 如果沒有命中任何元素，則將事件分派給視窗本身
    if (target != nullptr) {
        target->dispatchEvent(event);
    } else {
        dispatchEvent(event);
    }

    return target;
}

void Window::detachElementSubtree(Element* subtreeRoot) {
    // 互動狀態可能指向子樹中的任一後代，因此沿 parent 鏈判斷。
    const auto belongsToSubtree = [subtreeRoot](Element* element) {
        while (element != nullptr) {
            if (element == subtreeRoot) {
                return true;
            }
            element = element->parent();
        }
        return false;
    };

    if (belongsToSubtree(_focusedElement)) {
        // 在 parent 關係仍完整時派送 Blur，讓事件可以正常向上冒泡。
        setFocusedElement(nullptr);
    }

    if (belongsToSubtree(_hoveredElement)) {
        // 在拆除子樹前通知目前的懸停元素。
        UnhoverEvent event;
        _hoveredElement->dispatchEvent(event);
        _hoveredElement = nullptr;
    }

    if (belongsToSubtree(_mouseCapture)) {
        // 避免後續滑鼠事件被分派到已經脫離或遭銷毀的元素。
        _mouseCapture = nullptr;
    }
}

#pragma endregion  // Element interaction

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

    // 只有動畫狀態真正變化的 element 才會要求重繪。
    window->_rootElement->updateTree(std::chrono::milliseconds{CAPTURE_INTERVAL_MS});

    // 如果視窗需要捕捉內容到 ColorBuffer，且內容沒有被標記為 dirty，則進行捕捉
    if (window->_needsCapture && !window->_contentDirty) {
        const int previousWindow = glutGetWindow();

        // Timer 不會幫你選視窗，讀取 framebuffer 前要自行切換。
        glutSetWindow(windowId);

        window->_colorBuffer.capture(0, 0, window->_width, window->_height);
        window->_needsCapture = false;

        // glutGetWindow() 可能暫時回傳已被銷毀的視窗 ID。若切回這個失效視窗，
        // 下一次像素操作就會觸發 X11 BadDrawable 錯誤。
        if (previousWindow != 0 && previousWindow != windowId &&
            windows.find(previousWindow) != windows.end()) {
            glutSetWindow(previousWindow);
        }
    }

    // 重新啟動 timer，確保持續更新
    glutTimerFunc(CAPTURE_INTERVAL_MS, timerCallback, windowId);
}

#pragma endregion  // GLUT timer callback

#pragma region GLUT window callbacks

void Window::onClose() {
    if (modalWindow == this) {
        modalWindow = nullptr;
    }

    // 從管理列表中移除視窗，並將其 ID 設為 0
    windows.erase(_id);
    _id = 0;

    WindowCloseEvent event;

    dispatchEvent(event);
}

void Window::onReshape(int width, int height) {
    if (!isResizable() && (width != _width || height != _height)) {
        // 如果視窗不可調整大小，則恢復到原始大小
        glutReshapeWindow(_width, _height);
        return;
    }

    _width = width;
    _height = height;

    _colorBuffer.resize(width, height);  // 調整 ColorBuffer 的大小
    _contentDirty = true;
    _needsCapture = false;

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

    WindowResizeEvent event(width, height);

    dispatchEvent(event);

    requestLayout();
}

void Window::onVisibility(int state) {
    if (state == GLUT_VISIBLE) {
        WindowVisibleEvent event;
        dispatchEvent(event);
        requestRedisplay();
    } else if (state == GLUT_NOT_VISIBLE) {
        WindowHiddenEvent event;
        _contentDirty = true;
        _needsCapture = false;
        dispatchEvent(event);
    }
}

void Window::onDisplay() {
    // 清除顯示緩衝區
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 設置背景色為白色
    glClear(GL_COLOR_BUFFER_BIT);

    // 如果視窗內容被標記為 dirty，或者需要捕捉內容到 ColorBuffer，
    // 或者 ColorBuffer 的大小與視窗不匹配，則重新渲染視窗內容
    if (_contentDirty || _needsCapture || !_colorBuffer.matchesSize(_width, _height)) {
        render();  // 渲染視窗內容

        _contentDirty = false;
        _needsCapture = true;
    } else {
        // 直接從 ColorBuffer 恢復視窗內容，避免不必要的重繪
        _colorBuffer.restore();
    }

    // 提交繪圖命令
    glFlush();
}

#pragma endregion  // GLUT window callbacks

#pragma region GLUT keyboard callbacks

void Window::keyDownHandler(Key key, int x, int y) {
    if (!canReceiveInput(_id)) {
        activateModalWindow();
        return;
    }

    bool firstPress = _keyboardState._press(key);

    _mouseState._setPosition(x, y);

    KeyDownEvent event(_keyboardState, _mouseState, key, !firstPress);

    if (_focusedElement != nullptr) {
        _focusedElement->dispatchEvent(event);
    } else {
        dispatchEvent(event);
    }
}

void Window::keyUpHandler(Key key, int x, int y) {
    if (!canReceiveInput(_id)) {
        activateModalWindow();
        return;
    }

    _keyboardState._release(key);

    _mouseState._setPosition(x, y);

    KeyUpEvent event(_keyboardState, _mouseState, key);

    if (_focusedElement != nullptr) {
        _focusedElement->dispatchEvent(event);
    } else {
        dispatchEvent(event);
    }
}

void Window::onKeyboard(unsigned char key, int x, int y) {
    Key btn;
    if (_keyboardState.isDown(Key::LeftCtrl) || _keyboardState.isDown(Key::RightCtrl)) {
        btn = mapCharacterWithCtrl(key);
    } else {
        btn = mapCharacter(key);
    }

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyDownHandler(btn, x, y);

    if (!canReceiveInput(_id)) {
        activateModalWindow();
        return;
    }

    // 實際文字輸入事件
    const bool printable = key >= 32 && key != 127;

    const bool commandModifier = _keyboardState.isCtrlDown() || _keyboardState.isSuperDown();

    if (printable && !commandModifier) {
        TextInputEvent event(_keyboardState, _mouseState, std::string(1, static_cast<char>(key)));
        if (_focusedElement != nullptr) {
            _focusedElement->dispatchEvent(event);
        } else {
            dispatchEvent(event);
        }
    }
}

void Window::onKeyboardUp(unsigned char key, int x, int y) {
    Key btn;
    if (_keyboardState.isDown(Key::LeftCtrl) || _keyboardState.isDown(Key::RightCtrl)) {
        btn = mapCharacterWithCtrl(key);
    } else {
        btn = mapCharacter(key);
    }

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyUpHandler(btn, x, y);
}

void Window::onSpecial(int key, int x, int y) {
    auto btn = mapSpecialKey(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyDownHandler(btn, x, y);
}

void Window::onSpecialUp(int key, int x, int y) {
    auto btn = mapSpecialKey(key);

    // 未知按鈕，直接返回
    if (btn == Key::Unknown) {
        return;
    }

    keyUpHandler(btn, x, y);
}

#pragma endregion  // GLUT keyboard callbacks

#pragma region GLUT mouse callbacks

void Window::onMouse(int button, int state, int x, int y) {
    auto btn = mapMouseButton(button);

    // 未知按鈕，直接返回
    if (btn == MouseButton::Unknown) {
        return;
    }

    if (!canReceiveInput(_id)) {
        activateModalWindow();
        return;
    }

    _mouseState._setPosition(x, y);

    if (state == GLUT_DOWN) {
        _mouseState._press(btn);

        // 記錄滑鼠按下的位置，方便後續判斷點擊事件
        auto& press = _clickCandidate[btn];
        press.active = true;
        press.position = Point(x, y);

        MouseDownEvent event(_keyboardState, _mouseState, btn);

        auto target = dispatchMouseEvent<MouseDownEvent>(event);

        // 滑鼠事件仍送到最深層的命中元素；鍵盤焦點則沿 parent 往上尋找
        // 最近的可聚焦元素。
        Element* focusTarget = target;
        while (focusTarget != nullptr && !focusTarget->isFocusable()) {
            focusTarget = focusTarget->parent();
        }
        setFocusedElement(focusTarget);

    } else if (state == GLUT_UP) {
        _mouseState._release(btn);

        MouseUpEvent event(_keyboardState, _mouseState, btn);
        dispatchMouseEvent<MouseUpEvent>(event);

        // 判斷是否為點擊事件
        // 如果滑鼠按下和釋放的位置距離小於閾值，則認為是點擊事件
        auto& press = _clickCandidate[btn];
        if (press.active && abs(_mouseState.position() - press.position) <= CLICK_MOVE_THRESHOLD) {
            // 判斷是否為雙擊事件
            // 1. 上一次點擊事件有效
            // 2. 距離現在的時間小於閾值
            // 3. 上一次點擊事件的位置與現在的位置距離小於閾值

            auto& lastClick = _lastClicks[btn];
            auto now = std::chrono::steady_clock::now();
            const Point position = _mouseState.position();

            const bool isDoubleClick = lastClick.active &&
                                       now - lastClick.time <= DOUBLE_CLICK_TIME_THRESHOLD &&
                                       abs(position - lastClick.position) <= CLICK_MOVE_THRESHOLD;

            if (isDoubleClick) {
                lastClick.active = false;  // 重置上一次點擊事件，避免三擊事件被誤判為雙擊事件

                DoubleClickEvent doubleClickEvent(_keyboardState, _mouseState, btn);
                dispatchMouseEvent<DoubleClickEvent>(doubleClickEvent);
            } else {
                lastClick.active = true;
                lastClick.position = _mouseState.position();
                lastClick.time = now;

                ClickEvent clickEvent(_keyboardState, _mouseState, btn);
                dispatchMouseEvent<ClickEvent>(clickEvent);
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
    if (!canReceiveInput(_id)) {
        return;
    }

    _mouseState._setPosition(x, y);

    // 移動距離超過閾值，則取消所有滑鼠按下狀態，避免誤判為點擊事件
    for (auto& [button, press] : _clickCandidate) {
        if (press.active && abs(_mouseState.position() - press.position) > CLICK_MOVE_THRESHOLD) {
            press.active = false;
        }
    }

    // 處理滑鼠懸停事件

    // Hover 永遠依照實際位置判斷，
    // 不受 mouse capture 影響。
    Element* hoverTarget = dynamic_cast<Element*>(hitTest(_mouseState.position()));

    if (hoverTarget != _hoveredElement) {
        if (_hoveredElement != nullptr) {
            UnhoverEvent event;
            _hoveredElement->dispatchEvent(event);
        }

        _hoveredElement = hoverTarget;

        if (_hoveredElement != nullptr) {
            HoverEvent event;
            _hoveredElement->dispatchEvent(event);
        }
    }

    // MouseMove 本身則遵守 mouse capture
    MouseMoveEvent event(_keyboardState, _mouseState);
    dispatchMouseEvent<MouseMoveEvent>(event);
}

void Window::onMotion(int x, int y) {
    mouseMoveHandler(x, y);
}

void Window::onPassiveMotion(int x, int y) {
    mouseMoveHandler(x, y);
}

void Window::onEntry(int state) {
    if (!canReceiveInput(_id)) {
        return;
    }

    // 清除滑鼠按鈕狀態，避免在滑鼠進入或離開視窗時，按鈕狀態不一致。
    _mouseState._clear();

    if (state == GLUT_ENTERED) {
        WindowEnterEvent event;
        dispatchEvent(event);
    } else if (state == GLUT_LEFT) {
        WindowLeaveEvent event;
        dispatchEvent(event);
    }
}

#pragma endregion  // GLUT mouse callbacks

}  // namespace paint::ui
