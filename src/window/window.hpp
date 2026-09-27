#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

#include "element/element.hpp"
#include "event/event.hpp"
#include "input/input_state.hpp"

namespace paint {

const float CLICK_MOVE_THRESHOLD = 4.0f;  // 滑鼠移動距離超過此閾值，則取消點擊事件的判定。
const std::chrono::milliseconds DOUBLE_CLICK_TIME_THRESHOLD(300);  // 滑鼠雙擊的時間閾值，單位為毫秒。

struct ClickCandidate {
    bool active = false;
    Point position;
};

struct ClickHistory {
    bool active = false;
    Point position;
    std::chrono::steady_clock::time_point time;
};

class Window {
   public:
    Window(const std::string& title, int width, int height, bool resizable = true);
    virtual ~Window();

    // 禁止拷貝和賦值
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    int getId() const { return _id; }
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    const std::string& getTitle() const { return _title; }

    bool isResizable() const { return _resizable; }

    void setRootElement(ElementPtr rootElement) {
        _rootElement = std::move(rootElement);
        if (_rootElement) {
            // 設定根元素的 invalidate callback，當元素需要重新渲染時，呼叫此函式通知父視窗
            _rootElement->setInvalidateCallback([this]() { this->requestRedisplay(); });
        }
        requestRedisplay();
    }

   protected:
    // 回傳目前的輸入狀態。
    const KeyboardState& getKeyboardState() const { return _keyboardState; }
    const MouseState& getMouseState() const { return _mouseState; }

    virtual void onClose(const WindowCloseEvent& event) {}
    virtual void onResize(const WindowResizeEvent& event) {}
    virtual void onVisibilityChange(const WindowVisibilityEvent& event) {}
    virtual void onDisplay() {
        if (_rootElement) {
            _rootElement->render();
        }
    }

    virtual void onKeyDown(const KeyboardEvent& event) {
        if (_rootElement) {
            _rootElement->onKeyDown(event);
        }
    }

    virtual void onKeyUp(const KeyboardEvent& event) {
        if (_rootElement) {
            _rootElement->onKeyUp(event);
        }
    }

    virtual void onClick(const MouseClickEvent& event) {
        if (_rootElement) {
            _rootElement->onClick(event);
        }
    }

    virtual void onDoubleClick(const MouseClickEvent& event) {
        if (_rootElement) {
            _rootElement->onDoubleClick(event);
        }
    }

    virtual void onMouseDown(const MouseEvent& event) {
        if (_rootElement) {
            _rootElement->onMouseDown(event);
        }
    }

    virtual void onMouseUp(const MouseEvent& event) {
        if (_rootElement) {
            _rootElement->onMouseUp(event);
        }
    }

    virtual void onMouseMove(const MouseMoveEvent& event) {
        if (_rootElement) {
            _rootElement->onMouseMove(event);
        }
    }

    virtual void onMouseEnter(const MouseEnterEvent& event) {
        if (_rootElement) {
            _rootElement->onMouseEnter(event);
        }
    }

    virtual void onMouseLeave(const MouseLeaveEvent& event) {
        if (_rootElement) {
            _rootElement->onMouseLeave(event);
        }
    }

    void requestRedisplay();

   private:
    int _id = 0;  // GLUT window ID
    int _width, _height;
    std::string _title;

    ElementPtr _rootElement;  // 根元素

    bool _resizable;  // 是否允許調整視窗大小

    static KeyboardState _keyboardState;  // 全局的鍵盤狀態
    MouseState _mouseState;               // 視窗的滑鼠狀態

    std::unordered_map<MouseButton, ClickCandidate> _clickCandidate;  // 記錄滑鼠按下的位置，方便判斷點擊事件
    std::unordered_map<MouseButton, ClickHistory> _lastClicks;  // 記錄上一次滑鼠點擊事件，方便判斷雙擊事件

    static Window* getCurrentWindow();

    // GLUT callbacks
    static void closeCallback();

    static void reshapeCallback(int width, int height);
    static void visibilityCallback(int state);
    static void displayCallback();

    static void keyboardCallback(unsigned char key, int x, int y);
    static void keyboardUpCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void specialUpCallback(int key, int x, int y);

    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void passiveMotionCallback(int x, int y);
    static void entryCallback(int state);

    // Internal event handlers
    static void keyDownHandler(Key key, int x, int y);
    static void keyUpHandler(Key key, int x, int y);
    static void mouseMoveHandler(int x, int y);
};

using WindowPtr = std::unique_ptr<Window>;

}  // namespace paint
