#include "input/input_types.hpp"

#include <GL/freeglut.h>

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

Key mapCharacter(int glutKey) {
    switch (glutKey) {
    // Letters
    case 'a':
    case 'A':
        return Key::A;
    case 'b':
    case 'B':
        return Key::B;
    case 'c':
    case 'C':
        return Key::C;
    case 'd':
    case 'D':
        return Key::D;
    case 'e':
    case 'E':
        return Key::E;
    case 'f':
    case 'F':
        return Key::F;
    case 'g':
    case 'G':
        return Key::G;
    case 'h':
    case 'H':
        return Key::H;
    case 'i':
    case 'I':
        return Key::I;
    case 'j':
    case 'J':
        return Key::J;
    case 'k':
    case 'K':
        return Key::K;
    case 'l':
    case 'L':
        return Key::L;
    case 'm':
    case 'M':
        return Key::M;
    case 'n':
    case 'N':
        return Key::N;
    case 'o':
    case 'O':
        return Key::O;
    case 'p':
    case 'P':
        return Key::P;
    case 'q':
    case 'Q':
        return Key::Q;
    case 'r':
    case 'R':
        return Key::R;
    case 's':
    case 'S':
        return Key::S;
    case 't':
    case 'T':
        return Key::T;
    case 'u':
    case 'U':
        return Key::U;
    case 'v':
    case 'V':
        return Key::V;
    case 'w':
    case 'W':
        return Key::W;
    case 'x':
    case 'X':
        return Key::X;
    case 'y':
    case 'Y':
        return Key::Y;
    case 'z':
    case 'Z':
        return Key::Z;

        // Number row
    case '0':
    case ')':
        return Key::Digit0;
    case '1':
    case '!':
        return Key::Digit1;
    case '2':
    case '@':
        return Key::Digit2;
    case '3':
    case '#':
        return Key::Digit3;
    case '4':
    case '$':
        return Key::Digit4;
    case '5':
    case '%':
        return Key::Digit5;
    case '6':
    case '^':
        return Key::Digit6;
    case '7':
    case '&':
        return Key::Digit7;
    case '8':
    case '*':
        return Key::Digit8;
    case '9':
    case '(':
        return Key::Digit9;

        // Symbols
    case '-':
    case '_':
        return Key::Minus;
    case '=':
    case '+':
        return Key::Equal;
    case '[':
    case '{':
        return Key::LeftBracket;
    case ']':
    case '}':
        return Key::RightBracket;
    case '\\':
    case '|':
        return Key::Backslash;
    case ';':
    case ':':
        return Key::Semicolon;
    case '\'':
    case '"':
        return Key::Apostrophe;
    case ',':
    case '<':
        return Key::Comma;
    case '.':
    case '>':
        return Key::Period;
    case '/':
    case '?':
        return Key::Slash;
    case '`':
    case '~':
        return Key::GraveAccent;

    // Common keys
    case ' ':
        return Key::Space;
    case 27:  // ESC
        return Key::Escape;
    case '\r':
    case '\n':
        return Key::Enter;
    case '\b':
        return Key::Backspace;
    case '\t':
        return Key::Tab;

    // Delete
    case 127:
        return Key::Delete;

    // Unhandled keys
    default:
        return Key::Unknown;
    }
}

Key mapSpecialKey(int glutKey) {
    switch (glutKey) {
    // Navigation
    case GLUT_KEY_LEFT:
        return Key::Left;
    case GLUT_KEY_RIGHT:
        return Key::Right;
    case GLUT_KEY_UP:
        return Key::Up;
    case GLUT_KEY_DOWN:
        return Key::Down;

    case GLUT_KEY_PAGE_UP:
        return Key::PageUp;
    case GLUT_KEY_PAGE_DOWN:
        return Key::PageDown;
    case GLUT_KEY_HOME:
        return Key::Home;
    case GLUT_KEY_END:
        return Key::End;
    case GLUT_KEY_INSERT:
        return Key::Insert;

    // Function keys
    case GLUT_KEY_F1:
        return Key::F1;
    case GLUT_KEY_F2:
        return Key::F2;
    case GLUT_KEY_F3:
        return Key::F3;
    case GLUT_KEY_F4:
        return Key::F4;
    case GLUT_KEY_F5:
        return Key::F5;
    case GLUT_KEY_F6:
        return Key::F6;
    case GLUT_KEY_F7:
        return Key::F7;
    case GLUT_KEY_F8:
        return Key::F8;
    case GLUT_KEY_F9:
        return Key::F9;
    case GLUT_KEY_F10:
        return Key::F10;
    case GLUT_KEY_F11:
        return Key::F11;
    case GLUT_KEY_F12:
        return Key::F12;

    // Modifiers
    case GLUT_KEY_SHIFT_L:
        return Key::LeftShift;
    case GLUT_KEY_SHIFT_R:
        return Key::RightShift;
    case GLUT_KEY_CTRL_L:
        return Key::LeftCtrl;
    case GLUT_KEY_CTRL_R:
        return Key::RightCtrl;
    case GLUT_KEY_ALT_L:
        return Key::LeftAlt;
    case GLUT_KEY_ALT_R:
        return Key::RightAlt;
    case GLUT_KEY_SUPER_L:
        return Key::LeftSuper;
    case GLUT_KEY_SUPER_R:
        return Key::RightSuper;

    // Unhandled keys
    default:
        return Key::Unknown;
    }
}

MouseButton mapMouseButton(int glutButton) {
    switch (glutButton) {
    case GLUT_LEFT_BUTTON:
        return MouseButton::MouseLeft;
    case GLUT_MIDDLE_BUTTON:
        return MouseButton::MouseMiddle;
    case GLUT_RIGHT_BUTTON:
        return MouseButton::MouseRight;

    // Unhandled buttons
    default:
        return MouseButton::Unknown;
    }
}

}  // namespace paint
