#pragma once

#include <unordered_set>

#include "common/point.hpp"
#include "input/button.hpp"

namespace paint {

template <typename ButtonType>
class InputState {
   public:
    bool isDown(ButtonType button) const;
    bool isUp(ButtonType button) const;

    bool isAllDown(const std::unordered_set<ButtonType>& buttons) const;
    bool isExactlyDown(const std::unordered_set<ButtonType>& buttons) const;

   private:
    friend class Window;
    std::unordered_set<ButtonType> _down;

    // 回傳是否真的發生 up -> down。
    bool _press(ButtonType button);

    // 回傳是否真的發生 down -> up。
    bool _release(ButtonType button);

    void _clear();
};

class KeyboardState : public InputState<Key> {
   public:
    using InputState<Key>::InputState;

   private:
    friend class Window;
};

class MouseState : public InputState<MouseButton> {
   public:
    using InputState<MouseButton>::InputState;

    int x() const { return _mouseX; }
    int y() const { return _mouseY; }
    Point position() const { return Point(_mouseX, _mouseY); }
    void getPosition(int& x, int& y) const {
        x = _mouseX;
        y = _mouseY;
    }

   private:
    friend class Window;
    int _mouseX = 0;
    int _mouseY = 0;

    void _setMousePosition(int x, int y);
};

}  // namespace paint
