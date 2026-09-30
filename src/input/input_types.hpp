#pragma once

namespace paint {

enum class Key {
    Unknown,

    // Letters
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Number row
    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,

    // Symbols
    Minus,         // -
    Equal,         // =
    LeftBracket,   // [
    RightBracket,  // ]
    Backslash,     //
    Semicolon,     // ;
    Apostrophe,    // '
    Comma,         // ,
    Period,        // .
    Slash,         // /
    GraveAccent,   // `

    // Common keys
    Space,
    Escape,
    Enter,
    Backspace,
    Tab,

    // Modifiers
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,
    LeftSuper,  // Windows 上的 Windows 鍵、macOS 上的 Command 鍵
    RightSuper,

    // Function keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // Navigation
    Left,
    Right,
    Up,
    Down,

    PageUp,
    PageDown,
    Home,
    End,
    Insert,
    Delete,

    // Lock keys
    NumLock,
    CapsLock,    // unsupported by current GLUT backend
    ScrollLock,  // unsupported by current GLUT backend
};

// 用於表示修飾鍵，不分左右
enum class Mod {
    Shift,
    Ctrl,
    Alt,
    Super,
    Primary,
};

enum class MouseButton {
    Unknown,

    // Mouse
    MouseLeft,
    MouseMiddle,
    MouseRight,
    MouseButton4,
    MouseButton5,
};

enum class ButtonAction { Down, Up, Click, Unknown };

bool isShiftKey(Key key);
bool isCtrlKey(Key key);
bool isAltKey(Key key);
bool isSuperKey(Key key);
bool isModifierKey(Key key);
bool isPrimaryModifierKey(Key key);

bool operator==(Mod lhs, Key rhs);

inline bool operator!=(Mod lhs, Key rhs) {
    return !(lhs == rhs);
}

inline bool operator==(Key lhs, Mod rhs) {
    return rhs == lhs;
}

inline bool operator!=(Key lhs, Mod rhs) {
    return !(lhs == rhs);
}

}  // namespace paint
