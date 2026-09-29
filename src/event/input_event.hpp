#pragma once

#include "event/event.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"

namespace paint {

class InputEvent : public Event {
   public:
    InputEvent(const KeyboardState& keyboardState, const MouseState& mouseState)
        : _keyboardState(keyboardState), _mouseState(mouseState) {}

    const KeyboardState& getKeyboardState() const { return _keyboardState; }
    const MouseState& getMouseState() const { return _mouseState; }

    // 返回滑鼠目前的位置，方便繪圖工具使用。
    Point getPosition() const { return getMouseState().getPosition(); }

   private:
    const KeyboardState _keyboardState;
    const MouseState _mouseState;
};

class KeyboardEvent : public InputEvent {
   public:
    KeyboardEvent(Key key, ButtonAction action, const KeyboardState& inputState, const MouseState& mouseState,
                  bool isRepeat = false)
        : InputEvent(inputState, mouseState), _key(key), _action(action), _isRepeat(isRepeat) {}

    Key getKey() const { return _key; }
    ButtonAction getAction() const { return _action; }
    bool isRepeat() const { return _isRepeat; }

   private:
    Key _key;
    ButtonAction _action;
    bool _isRepeat;
};

class MouseEvent : public InputEvent {
   public:
    MouseEvent(MouseButton button, ButtonAction action, const KeyboardState& keyboardState,
               const MouseState& mouseState)
        : InputEvent(keyboardState, mouseState), _button(button), _action(action) {}

    MouseButton getButton() const { return _button; }
    ButtonAction getAction() const { return _action; }

   private:
    MouseButton _button;
    ButtonAction _action;
};

class MouseClickEvent : public InputEvent {
   public:
    MouseClickEvent(MouseButton button, int clickCount, const KeyboardState& keyboardState,
                    const MouseState& mouseState)
        : InputEvent(keyboardState, mouseState), _button(button), _clickCount(clickCount) {}

    MouseButton getButton() const { return _button; }
    int clickCount() const { return _clickCount; }

   private:
    MouseButton _button;
    int _clickCount = 0;
};

class MouseMoveEvent : public InputEvent {
   public:
    using InputEvent::InputEvent;
};

class MouseEnterEvent : public InputEvent {
   public:
    using InputEvent::InputEvent;
};

class MouseLeaveEvent : public InputEvent {
   public:
    using InputEvent::InputEvent;
};

}  // namespace paint
