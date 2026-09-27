#pragma once

#include "input/button.hpp"
#include "input/input_state.hpp"

namespace paint {

class Event {};

class ButtonEvent : public Event {
   public:
    ButtonEvent(Button button, ButtonAction action, const InputState& inputState, bool isRepeat = false)
        : _button(button), _action(action), _inputState(inputState), _isRepeat(isRepeat) {}

    Button button() const { return _button; }
    ButtonAction action() const { return _action; }
    const InputState& inputState() const { return _inputState; }
    bool isRepeat() const { return _isRepeat; }

   private:
    Button _button;
    ButtonAction _action;
    const InputState& _inputState;
    bool _isRepeat;
};

class KeyboardEvent : public ButtonEvent {
   public:
    using ButtonEvent::ButtonEvent;
};

class MouseEvent : public ButtonEvent {
   public:
    using ButtonEvent::ButtonEvent;
};

class MouseMoveEvent : public Event {
   public:
    MouseMoveEvent(int x, int y, const InputState& inputState) : _x(x), _y(y), _inputState(inputState) {}

    int x() const { return _x; }
    int y() const { return _y; }
    const InputState& inputState() const { return _inputState; }

   private:
    int _x;
    int _y;
    const InputState& _inputState;
};

class MouseScrollEvent : public Event {
   public:
    MouseScrollEvent(int xOffset, int yOffset, const InputState& inputState)
        : _xOffset(xOffset), _yOffset(yOffset), _inputState(inputState) {}

    int xOffset() const { return _xOffset; }
    int yOffset() const { return _yOffset; }
    const InputState& inputState() const { return _inputState; }

   private:
    int _xOffset;
    int _yOffset;
    const InputState& _inputState;
};

class WindowEvent : public Event {};

class WindowCloseEvent : public WindowEvent {};

class WindowResizeEvent : public WindowEvent {
   public:
    WindowResizeEvent(int width, int height) : _width(width), _height(height) {}

    int width() const { return _width; }
    int height() const { return _height; }

   private:
    int _width;
    int _height;
};

}  // namespace paint
