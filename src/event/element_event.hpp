#pragma once

#include "common/point.hpp"
#include "event/event.hpp"

namespace paint {

class ElementEvent : public Event {
   public:
    // 不支持冒泡，因為元素事件不應該傳播到其他元素或視窗。
    ElementEvent() : Event(false) {}
    explicit ElementEvent(bool bubbles) : Event(bubbles) {}
};

class ElementResizeEvent : public ElementEvent {
   public:
    ElementResizeEvent(int width, int height) : _width(width), _height(height) {}

    int width() const { return _width; }
    int height() const { return _height; }

   private:
    int _width;
    int _height;
};

class ElementMoveEvent : public ElementEvent {
   public:
    ElementMoveEvent(Point oldPosition, Point newPosition)
        : _oldPosition(oldPosition), _newPosition(newPosition) {}

    Point oldPosition() const { return _oldPosition; }
    Point newPosition() const { return _newPosition; }

   private:
    Point _oldPosition;
    Point _newPosition;
};

class FocusEvent : public ElementEvent {
   public:
    FocusEvent() : ElementEvent() {}
};

class BlurEvent : public ElementEvent {
   public:
    BlurEvent() : ElementEvent() {}
};

enum class ElementVisibilityState { Visible, Hidden };

class ElementVisibilityEvent : public ElementEvent {
   public:
    ElementVisibilityEvent(ElementVisibilityState state) : _state(state) {}

    ElementVisibilityState state() const { return _state; }

   private:
    ElementVisibilityState _state;
};

class ElementVisibleEvent : public ElementVisibilityEvent {
   public:
    ElementVisibleEvent() : ElementVisibilityEvent(ElementVisibilityState::Visible) {}
};

class ElementHiddenEvent : public ElementVisibilityEvent {
   public:
    ElementHiddenEvent() : ElementVisibilityEvent(ElementVisibilityState::Hidden) {}
};

}  // namespace paint
