#pragma once

#include "event/event.hpp"

namespace paint {

class ElementEvent : public Event {};

class ElementBoundsEvent : public ElementEvent {
   public:
    ElementBoundsEvent(int width, int height) : _width(width), _height(height) {}

    int width() const { return _width; }
    int height() const { return _height; }

   private:
    int _width;
    int _height;
};

}  // namespace paint
