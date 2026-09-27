#pragma once

namespace paint {

enum class Button {
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
    LeftSuper,
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

    // Mouse
    MouseLeft,
    MouseMiddle,
    MouseRight,
    MouseButton4,
    MouseButton5,
};

enum class ButtonAction { Down, Up };

}  // namespace paint
