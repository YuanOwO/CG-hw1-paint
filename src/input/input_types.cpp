#include "input/input_types.hpp"

namespace paint {

bool isShiftKey(Key key) {
    return key == Key::LeftShift || key == Key::RightShift;
}

bool isCtrlKey(Key key) {
    return key == Key::LeftCtrl || key == Key::RightCtrl;
}

bool isAltKey(Key key) {
    return key == Key::LeftAlt || key == Key::RightAlt;
}

bool isSuperKey(Key key) {
    return key == Key::LeftSuper || key == Key::RightSuper;
}

bool isModifierKey(Key key) {
    return isShiftKey(key) || isCtrlKey(key) || isAltKey(key) || isSuperKey(key);
}

bool isPrimaryModifierKey(Key key) {
#ifdef __APPLE__
    return isSuperKey(key);  // macOS 上的 Command 鍵是主要的修飾鍵
#else
    return isCtrlKey(key);  // Windows 與 Linux 上的 Ctrl 鍵是主要的修飾鍵
#endif
}

bool operator==(Mod lhs, Key rhs) {
    switch (lhs) {
    case Mod::Shift:
        return isShiftKey(rhs);
    case Mod::Ctrl:
        return isCtrlKey(rhs);
    case Mod::Alt:
        return isAltKey(rhs);
    case Mod::Super:
        return isSuperKey(rhs);
    case Mod::Primary:
        return isPrimaryModifierKey(rhs);
    default:
        return false;
    }
}

}  // namespace paint
