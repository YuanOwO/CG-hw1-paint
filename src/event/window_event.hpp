#pragma once

#include "event/event.hpp"

namespace paint {

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

class WindowVisibilityEvent : public WindowEvent {
   public:
    enum class WindowVisibilityState { Visible, Hidden };

    WindowVisibilityEvent(WindowVisibilityState state) : _state(state) {}

    WindowVisibilityState state() const { return _state; }

   private:
    WindowVisibilityState _state;
};

class WindowVisibleEvent : public WindowVisibilityEvent {
   public:
    WindowVisibleEvent() : WindowVisibilityEvent(WindowVisibilityState::Visible) {}
};

class WindowHiddenEvent : public WindowVisibilityEvent {
   public:
    WindowHiddenEvent() : WindowVisibilityEvent(WindowVisibilityState::Hidden) {}
};

class WindowEntryEvent : public WindowEvent {
   public:
    enum class WindowEnterState { Entered, Exited };

    WindowEntryEvent(WindowEnterState state) : _state(state) {}

    WindowEnterState state() const { return _state; }

   private:
    WindowEnterState _state;
};

class WindowEnterEvent : public WindowEntryEvent {
   public:
    WindowEnterEvent() : WindowEntryEvent(WindowEnterState::Entered) {}
};

class WindowLeaveEvent : public WindowEntryEvent {
   public:
    WindowLeaveEvent() : WindowEntryEvent(WindowEnterState::Exited) {}
};

}  // namespace paint
