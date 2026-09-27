#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event/event.hpp"
#include "input/input_state.hpp"

namespace paint {

class Window {
   public:
    Window(const std::string& title, int width, int height);
    virtual ~Window();

    // 禁止拷貝和賦值
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    int getId() const { return _id; }
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    const std::string& getTitle() const { return _title; }

   protected:
    const InputState& getInputState() const { return _inputState; }

    virtual void onClose(const WindowCloseEvent& event) {}
    virtual void onResize(const WindowResizeEvent& event) {}

    virtual void onKeyDown(const KeyboardEvent& event) {}
    virtual void onKeyUp(const KeyboardEvent& event) {}
    virtual void onMouseDown(const MouseEvent& event) {}
    virtual void onMouseUp(const MouseEvent& event) {}
    virtual void onMouseMove(const MouseMoveEvent& event) {}
    virtual void onMouseScroll(const MouseScrollEvent& event) {}

   private:
    int _id = 0;  // GLUT window ID
    int _width, _height;
    std::string _title;

    static InputState _inputState;  // 共享的輸入狀態

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
};

using WindowPtr = std::unique_ptr<Window>;

}  // namespace paint
