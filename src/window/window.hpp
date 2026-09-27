#pragma once

#include <memory>
#include <string>

#include "event/hid_event.hpp"
#include "event/window_event.hpp"
#include "input/input_state.hpp"

namespace paint {

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

   protected:
    // 回傳目前的輸入狀態。
    const KeyboardState& getKeyboardState() const { return _keyboardState; }
    const MouseState& getMouseState() const { return _mouseState; }

    virtual void onClose(const WindowCloseEvent& event) {}
    virtual void onResize(const WindowResizeEvent& event) {}
    virtual void onVisibilityChange(const WindowVisibilityEvent& event) {}
    virtual void onDisplay() {}

    virtual void onKeyDown(const KeyboardEvent& event) {}
    virtual void onKeyUp(const KeyboardEvent& event) {}
    virtual void onMouseDown(const MouseEvent& event) {}
    virtual void onMouseUp(const MouseEvent& event) {}
    virtual void onMouseMove(const MouseMoveEvent& event) {}
    virtual void onMouseEnter(const MouseEnterEvent& event) {}
    virtual void onMouseLeave(const MouseLeaveEvent& event) {}

    void requestRedisplay();

   private:
    int _id = 0;  // GLUT window ID
    int _width, _height;
    std::string _title;

    bool _resizable;  // 是否允許調整視窗大小

    static KeyboardState _keyboardState;  // 全局的鍵盤狀態
    MouseState _mouseState;               // 視窗的滑鼠狀態

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
