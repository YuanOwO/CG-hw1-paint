#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "event/event_target.hpp"
#include "event/events.hpp"
#include "input/input_state.hpp"
#include "render/color_buffer.hpp"
#include "render/render_context.hpp"
#include "ui/element/element.hpp"

namespace paint {

const int CAPTURE_RATE = 60;                          // 每秒幀數
const int CAPTURE_INTERVAL_MS = 1000 / CAPTURE_RATE;  // 每幀的時間間隔，單位為毫秒
const float CLICK_MOVE_THRESHOLD = 4.0f;              // 滑鼠移動距離超過此閾值，則取消點擊事件的判定。
const std::chrono::milliseconds DOUBLE_CLICK_TIME_THRESHOLD(300);  // 滑鼠雙擊的時間閾值，單位為毫秒。

class Window : public EventTarget {
   public:
    struct ClickCandidate {
        bool active = false;
        Point position;
    };

    struct ClickHistory {
        bool active = false;
        Point position;
        std::chrono::steady_clock::time_point time;
    };

    Window(const std::string& title, int width, int height, bool resizable = true);
    virtual ~Window();

    // 禁止拷貝與移動操作，確保元素的唯一性
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    int id() const { return _id; }
    int width() const { return _width; }
    int height() const { return _height; }
    const std::string& title() const { return _title; }

    bool isResizable() const { return _resizable; }

    void setRootElement(std::unique_ptr<Element> rootElement);

   protected:
    friend class Element;  // 允許 Element 訪問 Window 的私有成員

    // 回傳目前的輸入狀態。
    const KeyboardState& keyboardState() const { return _keyboardState; }
    const MouseState& mouseState() const { return _mouseState; }

    // 當視窗需要重新渲染內容時，呼叫此函式。子類別可以覆寫此函式來實現自定義的渲染邏輯。
    virtual void render() {
        RenderContext context;

        if (_rootElement) {
            _rootElement->render(context);
        }
    }

    void requestRedisplay();

    // 請求顯示現有快取，不將內容標記為 dirty；快取尚未就緒時沿用正常渲染流程。
    void requestCachedRedisplay();

    void setFocusedElement(Element* element);

   private:
    int _id = 0;  // GLUT window ID
    int _width;
    int _height;
    std::string _title;

    bool _resizable;  // 是否允許調整視窗大小

    bool _contentDirty = true;   // 是否需要重新渲染視窗內容
    bool _needsCapture = false;  // 是否需要捕捉視窗內容到 ColorBuffer
    ColorBuffer _colorBuffer;

    std::unique_ptr<Element> _rootElement;  // 根元素

    Element* _focusedElement = nullptr;  // 當前獲得鍵盤焦點的元素
    Element* _hoveredElement = nullptr;  // 當前滑鼠懸停的元素
    Element* _mouseCapture = nullptr;    // 當前捕捉滑鼠事件的元素

    static KeyboardState _keyboardState;  // 全局的鍵盤狀態
    MouseState _mouseState;               // 視窗的滑鼠狀態

    std::unordered_map<MouseButton, ClickCandidate> _clickCandidate;  // 記錄滑鼠按下的位置，方便判斷點擊事件
    std::unordered_map<MouseButton, ClickHistory> _lastClicks;  // 記錄上一次滑鼠點擊事件，方便判斷雙擊事件

    Element* hitTest(Point point) {
        if (_rootElement) {
            return _rootElement->hitTest(point);
        }
        return nullptr;
    }

    template <typename EventType, typename... Args>
    Element* dispatchMouseEvent(Args&&... args) {
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

    void captureMouse(Element* element) { _mouseCapture = element; }
    void releaseMouseCapure() { _mouseCapture = nullptr; }
    void releaseMouseCaptureIf(Element* element) {
        if (_mouseCapture == element) {
            _mouseCapture = nullptr;
        }
    }

    // 元素子樹即將脫離視窗時，清除所有指向該子樹的互動狀態。
    void detachElementSubtree(Element* subtreeRoot);

    static Window* currentWindow();

    // GLUT callbacks
    static void timerCallback(int windowId);

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

}  // namespace paint
