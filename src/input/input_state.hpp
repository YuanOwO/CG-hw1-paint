#pragma once

#include <unordered_set>

#include "common/point.hpp"
#include "input/input_types.hpp"

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

    using InputState<Key>::isDown;  // 繼承父類別的 isDown 方法，避免被覆蓋
    using InputState<Key>::isUp;    // 繼承父類別的 isUp 方法，避免被覆蓋
    bool isDown(Mod mod) const;     // 判斷指定的修飾鍵是否按下

    bool isShiftDown() const { return isDown(Key::LeftShift) || isDown(Key::RightShift); }
    bool isCtrlDown() const { return isDown(Key::LeftCtrl) || isDown(Key::RightCtrl); }
    bool isAltDown() const { return isDown(Key::LeftAlt) || isDown(Key::RightAlt); }
    bool isSuperDown() const { return isDown(Key::LeftSuper) || isDown(Key::RightSuper); }
    bool isModifierDown() const { return isShiftDown() || isCtrlDown() || isAltDown() || isSuperDown(); }

    bool isPrimaryModifierDown() const {
#ifdef __APPLE__
        return isSuperDown();  // macOS 上的 Command 鍵是主要的修飾鍵
#else
        return isCtrlDown();  // Windows 與 Linux 上的 Ctrl 鍵是主要的修飾鍵
#endif
    }

   private:
    friend class Window;
};

class MouseState : public InputState<MouseButton> {
   public:
    using InputState<MouseButton>::InputState;

    int x() const { return _mouseX; }
    int y() const { return _mouseY; }
    Point position() const { return Point(_mouseX, _mouseY); }

   private:
    friend class Window;

    int _mouseX = 0;
    int _mouseY = 0;

    void _setPosition(int x, int y);
};

}  // namespace paint
