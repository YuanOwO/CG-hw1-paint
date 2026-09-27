#pragma once

#include <unordered_set>

#include "input/button.hpp"

namespace paint {

class InputState {
   public:
    bool isDown(Button button) const;
    bool isUp(Button button) const;

    bool isAllDown(const std::unordered_set<Button>& buttons) const;
    bool isExactlyDown(const std::unordered_set<Button>& buttons) const;

    int mouseX() const { return _mouseX; }
    int mouseY() const { return _mouseY; }

   private:
    friend class Window;

    int _mouseX = 0;
    int _mouseY = 0;

    std::unordered_set<Button> _down;

    void _setMousePosition(int x, int y) {
        _mouseX = x;
        _mouseY = y;
    }

    // 回傳是否真的發生 up -> down。
    bool _press(Button button);

    // 回傳是否真的發生 down -> up。
    bool _release(Button button);

    void _clear();
};

}  // namespace paint
