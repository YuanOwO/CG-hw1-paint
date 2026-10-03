#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "event/events.hpp"
#include "input/input_state.hpp"
#include "render/color_buffer.hpp"
#include "render/render_context.hpp"
#include "ui/event_target.hpp"

namespace paint::app {

class Application;

}

namespace paint::ui {

const int CAPTURE_RATE = 60;
const int CAPTURE_INTERVAL_MS = 1000 / CAPTURE_RATE;
const float CLICK_MOVE_THRESHOLD = 4.0f;
const std::chrono::milliseconds DOUBLE_CLICK_TIME_THRESHOLD(300);

class Element;
class RootElement;

class Window : public EventTarget {
   public:
    Window(app::Application& app, const std::string& title, int width, int height, bool resizable = true);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Properties

    int id() const { return _id; }

    int width() const { return _width; }
    int height() const { return _height; }

    const std::string& title() const { return _title; }

    bool isResizable() const { return _resizable; }

    bool shouldClose() const { return _shouldClose; }

    // Content

    void setContent(std::unique_ptr<Element> content);

   protected:
    friend class Element;

    // Properties

    app::Application& app() { return _app; }

    void setTitle(const std::string& title);

    void close();

    // Input state

    const KeyboardState& keyboardState() const { return _keyboardState; }

    const MouseState& mouseState() const { return _mouseState; }

    void resetInputState();
    void setFocusedElement(Element* element);

    // Geometry

    void requestLayout();
    void ensureLayout();

    // Rendering

    void render();

    void requestRedisplay();
    void requestCachedRedisplay();

   private:
    struct ClickCandidate {
        bool active = false;
        Point position;
    };

    struct ClickHistory {
        bool active = false;
        Point position;
        std::chrono::steady_clock::time_point time;
    };

    // Element interaction

    Element* hitTest(Point point);

    template <typename EventType, typename... Args>
    Element* dispatchMouseEvent(Args&&... args);

    void captureMouse(Element* element) { _mouseCapture = element; }

    void releaseMouseCapture() { _mouseCapture = nullptr; }

    void releaseMouseCaptureIf(Element* element) {
        if (_mouseCapture == element) {
            _mouseCapture = nullptr;
        }
    }

    void detachElementSubtree(Element* subtreeRoot);

    // Window lookup

    static Window* currentWindow();

    // Window properties

    int _id = 0;

    int _width;
    int _height;

    std::string _title;
    bool _resizable;

    bool _shouldClose = false;

    app::Application& _app;

    // Geometry

    void updateLayout();  // 更新佈局

    bool _layoutDirty = true;

    // Rendering

    bool _contentDirty = true;
    bool _needsCapture = false;

    ColorBuffer _colorBuffer;

    // Element tree

    std::unique_ptr<RootElement> _rootElement;

    // Interaction state

    Element* _focusedElement = nullptr;
    Element* _hoveredElement = nullptr;
    Element* _mouseCapture = nullptr;

    // Input state

    static KeyboardState _keyboardState;
    MouseState _mouseState;

    // Mouse click state

    std::unordered_map<MouseButton, ClickCandidate> _clickCandidate;
    std::unordered_map<MouseButton, ClickHistory> _lastClicks;

    // Internal event handlers

    static void keyDownHandler(Key key, int x, int y);
    static void keyUpHandler(Key key, int x, int y);
    static void mouseMoveHandler(int x, int y);

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
};

}  // namespace paint::ui
