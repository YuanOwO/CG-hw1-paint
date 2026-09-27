#pragma once

#include "event/event.hpp"

namespace paint {

class HIDEvent : public Event {
   public:
    HIDEvent(const KeyboardState& keyboardState, const MouseState& mouseState)
        : _keyboardState(keyboardState), _mouseState(mouseState) {}

    const KeyboardState& keyboardState() const { return _keyboardState; }
    const MouseState& mouseState() const { return _mouseState; }

   private:
    const KeyboardState _keyboardState;
    const MouseState _mouseState;
};

class KeyboardEvent : public HIDEvent {
   public:
    KeyboardEvent(Key key, ButtonAction action, const KeyboardState& inputState, const MouseState& mouseState,
                  bool isRepeat = false)
        : HIDEvent(inputState, mouseState), _key(key), _action(action), _isRepeat(isRepeat) {}

    Key key() const { return _key; }
    ButtonAction action() const { return _action; }
    bool isRepeat() const { return _isRepeat; }

   private:
    Key _key;
    ButtonAction _action;
    bool _isRepeat;
};

class MouseEvent : public HIDEvent {
   public:
    MouseEvent(MouseButton button, ButtonAction action, const KeyboardState& keyboardState,
               const MouseState& mouseState)
        : HIDEvent(keyboardState, mouseState), _button(button), _action(action) {}

    MouseButton button() const { return _button; }
    ButtonAction action() const { return _action; }

   private:
    MouseButton _button;
    ButtonAction _action;
};

class MouseClickEvent : public HIDEvent {
   public:
    MouseClickEvent(MouseButton button, int clickCount, const KeyboardState& keyboardState,
                    const MouseState& mouseState)
        : HIDEvent(keyboardState, mouseState), _button(button), _clickCount(clickCount) {}

    MouseButton button() const { return _button; }
    int clickCount() const { return _clickCount; }

   private:
    MouseButton _button;
    int _clickCount = 0;
};

class MouseMoveEvent : public HIDEvent {
   public:
    using HIDEvent::HIDEvent;

    int x() const { return mouseState().x(); }
    int y() const { return mouseState().y(); }
    Point position() const { return mouseState().position(); }

   private:
    int _x;
    int _y;
};

class MouseEnterEvent : public HIDEvent {
   public:
    using HIDEvent::HIDEvent;
};

class MouseLeaveEvent : public HIDEvent {
   public:
    using HIDEvent::HIDEvent;
};

}  // namespace paint
