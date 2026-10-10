// macOS 的事件攔截器，說明見 keyboard.hpp。

#ifdef __APPLE__

#  include "platform/apple/keyboard.hpp"

#  include <Carbon/Carbon.h>
#  include <GLUT/glut.h>

#  include <iterator>
#  include <unordered_map>

#  include "platform/apple/cocoa.hpp"
#  include "platform/platform_internal.hpp"

namespace paint::platform::keyboard {

namespace {

#  pragma region Key dispatch

// 以 GLUT callback 的方式呼叫 handler：呼叫前將目標視窗設為目前視窗，結束後切回

// 將 window 設為目前視窗，回傳原本的目前視窗
int makeCurrent(int window) {
    const int previous = glutGetWindow();
    if (previous != window) {
        glutSetWindow(window);
    }
    return previous;
}

// 切回原本的目前視窗。若原本的視窗已經關閉，則不切換
void restoreCurrent(int previous) {
    if (previous != 0 && previous != glutGetWindow() && findWindowHandler(previous) != nullptr) {
        glutSetWindow(previous);
    }
}

void sendSpecialKey(int window, int key, bool down, int x, int y) {
    WindowHandler* handler = findWindowHandler(window);
    if (handler == nullptr) {
        return;
    }

    const int previous = makeCurrent(window);

    if (down) {
        handler->onSpecial(key, x, y);
    } else {
        handler->onSpecialUp(key, x, y);
    }

    restoreCurrent(previous);
}

void sendKey(int window, unsigned char key, bool down, int x, int y) {
    WindowHandler* handler = findWindowHandler(window);
    if (handler == nullptr) {
        return;
    }

    const int previous = makeCurrent(window);

    if (down) {
        handler->onKeyboard(key, x, y);
    } else {
        handler->onKeyboardUp(key, x, y);
    }

    restoreCurrent(previous);
}

#  pragma endregion  // Key dispatch

#  pragma region Modifier keys

// NSEventModifierFlags
constexpr unsigned long FLAG_SHIFT = 1ul << 17;
constexpr unsigned long FLAG_COMMAND = 1ul << 20;

// 區分左右鍵的位元（IOKit 的 NX_DEVICE*KEYMASK）
constexpr unsigned long DEVICE_LCTL = 0x0001;
constexpr unsigned long DEVICE_LSHIFT = 0x0002;
constexpr unsigned long DEVICE_RSHIFT = 0x0004;
constexpr unsigned long DEVICE_LCMD = 0x0008;
constexpr unsigned long DEVICE_RCMD = 0x0010;
constexpr unsigned long DEVICE_LALT = 0x0020;
constexpr unsigned long DEVICE_RALT = 0x0040;
constexpr unsigned long DEVICE_RCTL = 0x2000;

struct ModifierKey {
    int key;             // KEY_SHIFT_L 等
    unsigned long mask;  // 按住時 modifierFlags 中會設定的位元
};

// 實體按鍵 → 修飾鍵
const std::unordered_map<unsigned short, ModifierKey> modifierKeys = {
    {kVK_Shift,        {KEY_SHIFT_L, DEVICE_LSHIFT}},
    {kVK_RightShift,   {KEY_SHIFT_R, DEVICE_RSHIFT}},
    {kVK_Control,      {KEY_CTRL_L, DEVICE_LCTL}   },
    {kVK_RightControl, {KEY_CTRL_R, DEVICE_RCTL}   },
    {kVK_Option,       {KEY_ALT_L, DEVICE_LALT}    },
    {kVK_RightOption,  {KEY_ALT_R, DEVICE_RALT}    },
    {kVK_Command,      {KEY_SUPER_L, DEVICE_LCMD}  },
    {kVK_RightCommand, {KEY_SUPER_R, DEVICE_RCMD}  },
};

// 按住中的修飾鍵 → 收到按下事件的視窗。放開事件送給同一個視窗，即使焦點已經改變
std::unordered_map<int, int> heldModifiers;

void handleModifiersChanged(const cocoa::KeyEvent& event) {
    auto it = modifierKeys.find(event.keyCode);
    if (it == modifierKeys.end()) {
        return;
    }

    const ModifierKey& modifier = it->second;

    if ((event.modifierFlags & modifier.mask) != 0) {
        heldModifiers[modifier.key] = event.window;
        sendSpecialKey(event.window, modifier.key, true, event.x, event.y);
    } else if (auto held = heldModifiers.find(modifier.key); held != heldModifiers.end()) {
        const int window = held->second;
        heldModifiers.erase(held);
        sendSpecialKey(window, modifier.key, false, event.x, event.y);
    }
}

// 切換到其他程式時收不到修飾鍵的放開事件，因此全部放開
void releaseHeldModifiers() {
    const auto held = heldModifiers;
    heldModifiers.clear();

    for (const auto& [key, window] : held) {
        sendSpecialKey(window, key, false, 0, 0);
    }
}

#  pragma endregion  // Modifier keys

#  pragma region Character keys

// 依實體按鍵，用 ASCII 鍵盤配置換算字元，不受注音等輸入法影響。無法換算時回傳 0
UniChar asciiCharacter(unsigned short keyCode, bool shift) {
    TISInputSourceRef source = TISCopyCurrentASCIICapableKeyboardLayoutInputSource();
    if (source == nullptr) {
        return 0;
    }

    UniChar result = 0;

    auto data = static_cast<CFDataRef>(TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));
    if (data != nullptr) {
        auto layout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(data));
        const UInt32 modifiers = shift ? (shiftKey >> 8) : 0;
        UInt32 deadKeyState = 0;
        UniChar chars[4];
        UniCharCount length = 0;

        UCKeyTranslate(layout, keyCode, kUCKeyActionDown, modifiers, LMGetKbdType(),
                       kUCKeyTranslateNoDeadKeysBit, &deadKeyState, 4, &length, chars);

        if (length > 0) {
            result = chars[0];
        }
    }

    CFRelease(source);
    return result;
}

// 回傳 true 表示已處理，不再交給 Apple GLUT
bool handleKey(const cocoa::KeyEvent& event) {
    const bool down = event.type == cocoa::KeyEvent::Type::KeyDown;

    // Command 組合鍵
    if ((event.modifierFlags & FLAG_COMMAND) != 0) {
        const UniChar character = asciiCharacter(event.keyCode, (event.modifierFlags & FLAG_SHIFT) != 0);

        // 保留 Cmd+Q（結束）、Cmd+H（隱藏）、Cmd+M（最小化）給系統
        if (character == 0 || character >= 128 || character == 'q' || character == 'h' || character == 'm') {
            return false;
        }

        sendKey(event.window, static_cast<unsigned char>(character), down, event.x, event.y);
        return true;
    }

    // Backspace / Delete：送出與 FreeGLUT 相同的字元
    switch (event.keyCode) {
    case kVK_Delete:  // Backspace
        sendKey(event.window, '\b', down, event.x, event.y);
        return true;
    case kVK_ForwardDelete:
        sendKey(event.window, 127, down, event.x, event.y);
        return true;
    default:
        return false;
    }
}

#  pragma endregion  // Character keys

// 事件攔截器：在 Apple GLUT 處理之前收到所有 GLUT 視窗的鍵盤事件
bool handleKeyEvent(const cocoa::KeyEvent& event) {
    if (event.type == cocoa::KeyEvent::Type::ModifiersChanged) {
        handleModifiersChanged(event);
        return false;  // Apple GLUT 本身不處理修飾鍵，照常交給它
    }

    return handleKey(event);
}

}  // namespace

void install() {
    cocoa::installKeyEventMonitor(handleKeyEvent);
    cocoa::observeDeactivation(releaseHeldModifiers);
}

void forgetWindow(int window) {
    for (auto it = heldModifiers.begin(); it != heldModifiers.end();) {
        it = it->second == window ? heldModifiers.erase(it) : std::next(it);
    }
}

}  // namespace paint::platform::keyboard

#endif
