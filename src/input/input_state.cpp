#include "input/input_state.hpp"

namespace paint {

bool InputState::isDown(Button button) const {
    return _down.find(button) != _down.end();
}

bool InputState::isUp(Button button) const {
    return !isDown(button);
}

bool InputState::isAllDown(const std::unordered_set<Button>& buttons) const {
    for (const auto& button : buttons) {
        if (!isDown(button)) {
            return false;
        }
    }
    return true;
}

bool InputState::isExactlyDown(const std::unordered_set<Button>& buttons) const {
    return _down == buttons;
}

bool InputState::_press(Button button) {
    if (button == Button::Unknown) {
        return false;  // 忽略未知按鈕
    }

    auto result = _down.insert(button);
    return result.second;
}

bool InputState::_release(Button button) {
    if (button == Button::Unknown) {
        return false;  // 忽略未知按鈕
    }

    auto result = _down.erase(button);
    return result > 0;
}

void InputState::_clear() {
    _down.clear();
}

}  // namespace paint
