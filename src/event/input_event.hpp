#pragma once

#include "event/event.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"

namespace paint {

class InputEvent : public Event {
   public:
    //  預設 InputEvent 為可冒泡事件，方便在元素樹中傳播。
    InputEvent(const KeyboardState& keyboardState, const MouseState& mouseState)
        : Event(true), _keyboardState(keyboardState), _mouseState(mouseState) {}

    const KeyboardState& keyboardState() const { return _keyboardState; }
    const MouseState& mouseState() const { return _mouseState; }

    // 返回滑鼠目前的位置，方便繪圖工具使用。
    Point position() const { return mouseState().position(); }

   private:
    // 保存當前的鍵盤與滑鼠狀態，方便事件處理時使用。
    const KeyboardState _keyboardState;
    const MouseState _mouseState;
};

// ==========================
// Keyboard
// ==========================

class KeyboardEvent : public InputEvent {
   public:
    KeyboardEvent(const KeyboardState& inputState, const MouseState& mouseState, Key key, ButtonAction action,
                  bool isRepeat = false)
        : InputEvent(inputState, mouseState), _key(key), _action(action), _isRepeat(isRepeat) {}

    Key key() const { return _key; }
    ButtonAction action() const { return _action; }

    bool isRepeat() const { return _isRepeat; }

   private:
    Key _key;
    ButtonAction _action;
    bool _isRepeat;
};

class KeyDownEvent : public KeyboardEvent {
   public:
    KeyDownEvent(const KeyboardState& inputState, const MouseState& mouseState, Key key,
                 bool isRepeat = false)
        : KeyboardEvent(inputState, mouseState, key, ButtonAction::Down, isRepeat) {}
};

class KeyUpEvent : public KeyboardEvent {
   public:
    KeyUpEvent(const KeyboardState& inputState, const MouseState& mouseState, Key key)
        : KeyboardEvent(inputState, mouseState, key, ButtonAction::Up) {}
};

// ==========================
// Mouse
// ==========================

class MouseEvent : public InputEvent {
   public:
    MouseEvent(const KeyboardState& keyboardState, const MouseState& mouseState)
        : InputEvent(keyboardState, mouseState) {}
};

class MouseButtonEvent : public MouseEvent {
   public:
    MouseButtonEvent(const KeyboardState& keyboardState, const MouseState& mouseState, MouseButton button,
                     ButtonAction action)
        : MouseEvent(keyboardState, mouseState), _button(button), _action(action) {}

    MouseButton button() const { return _button; }
    ButtonAction action() const { return _action; }

   private:
    MouseButton _button;
    ButtonAction _action;
};

class MouseDownEvent : public MouseButtonEvent {
   public:
    MouseDownEvent(const KeyboardState& keyboardState, const MouseState& mouseState, MouseButton button)
        : MouseButtonEvent(keyboardState, mouseState, button, ButtonAction::Down) {}
};

class MouseUpEvent : public MouseButtonEvent {
   public:
    MouseUpEvent(const KeyboardState& keyboardState, const MouseState& mouseState, MouseButton button)
        : MouseButtonEvent(keyboardState, mouseState, button, ButtonAction::Up) {}
};

class ClickEvent : public MouseButtonEvent {
   public:
    ClickEvent(const KeyboardState& keyboardState, const MouseState& mouseState, MouseButton button,
               int clickCount = 1)
        : MouseButtonEvent(keyboardState, mouseState, button, ButtonAction::Click), _clickCount(clickCount) {}

    int clickCount() const { return _clickCount; }

   private:
    int _clickCount;
};

class DoubleClickEvent : public ClickEvent {
   public:
    DoubleClickEvent(const KeyboardState& keyboardState, const MouseState& mouseState, MouseButton button)
        : ClickEvent(keyboardState, mouseState, button, 2) {}
};

class MouseMoveEvent : public MouseEvent {
   public:
    using MouseEvent::MouseEvent;
};

}  // namespace paint
