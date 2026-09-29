#include "input/input_state.hpp"

namespace paint {

template <typename ButtonType>
bool InputState<ButtonType>::isDown(ButtonType button) const {
    return _down.find(button) != _down.end();
}

template <typename ButtonType>
bool InputState<ButtonType>::isUp(ButtonType button) const {
    return !isDown(button);
}

template <typename ButtonType>
bool InputState<ButtonType>::isAllDown(const std::unordered_set<ButtonType>& buttons) const {
    for (const auto& button : buttons) {
        if (!isDown(button)) {
            return false;
        }
    }
    return true;
}

template <typename ButtonType>
bool InputState<ButtonType>::isExactlyDown(const std::unordered_set<ButtonType>& buttons) const {
    return _down == buttons;
}

template <typename ButtonType>
bool InputState<ButtonType>::_press(ButtonType button) {
    if (button == ButtonType::Unknown) {
        return false;  // 忽略未知按鈕
    }

    auto result = _down.insert(button);
    return result.second;
}

template <typename ButtonType>
bool InputState<ButtonType>::_release(ButtonType button) {
    if (button == ButtonType::Unknown) {
        return false;  // 忽略未知按鈕
    }

    auto result = _down.erase(button);
    return result > 0;
}

template <typename ButtonType>
void InputState<ButtonType>::_clear() {
    _down.clear();
}

// 模板定義留在此檔案，明確產生鍵盤與滑鼠使用的實例。
template class InputState<Key>;
template class InputState<MouseButton>;

void MouseState::_setPosition(int x, int y) {
    _mouseX = x;
    _mouseY = y;
}

}  // namespace paint
