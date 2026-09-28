#pragma once

#include "event/event.hpp"

namespace paint {

class WindowEvent : public IEvent {};

class WindowCloseEvent : public WindowEvent {};

class WindowResizeEvent : public WindowEvent {
   public:
    WindowResizeEvent(int width, int height) : _width(width), _height(height) {}

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

   private:
    int _width;
    int _height;
};

class WindowVisibilityEvent : public WindowEvent {
   public:
    enum class WindowVisibilityState { Visible, Hidden };

    WindowVisibilityEvent(WindowVisibilityState state) : _state(state) {}

    WindowVisibilityState getState() const { return _state; }

   private:
    WindowVisibilityState _state;
};

}  // namespace paint
