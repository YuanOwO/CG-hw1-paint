#pragma once

// 與 macOS Cocoa 框架溝通的部分（Objective-C，實作在 cocoa.mm、context_menu.mm）。
// 只負責把 macOS 的事件與視窗轉成 C++ 的資料，處理邏輯在 keyboard.cpp、platform.cpp。

namespace paint::platform::cocoa {

struct KeyEvent {
    enum class Type {
        KeyDown,
        KeyUp,
        ModifiersChanged,
    };

    Type type;
    unsigned short keyCode;       // 實體按鍵（kVK_*）
    unsigned long modifierFlags;  // NSEventModifierFlags，含區分左右鍵的位元
    int window;                   // 收到事件的 GLUT 視窗
    int x, y;                     // 滑鼠在視窗中的位置，與 GLUT callback 相同的座標系
};

// 回傳 true 表示事件已處理，不再交給 Apple GLUT
using KeyEventHandler = bool (*)(const KeyEvent& event);

// 在 Apple GLUT 處理之前攔截鍵盤事件。只會收到 GLUT 視窗的事件。
void installKeyEventMonitor(KeyEventHandler handler);

// 程式切換到背景時呼叫 callback
void observeDeactivation(void (*callback)());

// 程式切換到前景時呼叫 callback；呼叫此函數時已在前景，則立即呼叫一次
void observeActivation(void (*callback)());

// 程式即將切換到背景或結束時呼叫 callback（此時仍在前景）
void observeWillDeactivation(void (*callback)());

// 建立 GLUT 視窗，並記住對應的 NSWindow
int createWindow(const char* title);

// 忘記視窗對應的 NSWindow
void forgetWindow(int window);

// 右鍵選單不附加其他 App 提供的服務項目（context_menu.mm）
void disableContextMenuPlugIns();

}  // namespace paint::platform::cocoa
