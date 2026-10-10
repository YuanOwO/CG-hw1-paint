#pragma once

// macOS 的事件攔截器：補上 Apple GLUT 與 FreeGLUT 在鍵盤上的差異。
//   - 不會送出修飾鍵事件 → 攔截修飾鍵的變化，送出 onSpecial / onSpecialUp
//   - Command 組合鍵會被應用程式選單攔截（例如 Cmd+S 會儲存 TIFF），且不回報 Command
//       → 攔截後依實體按鍵換算字元，送出 onKeyboard / onKeyboardUp
//   - Backspace 送出 0x7F、Delete 送出 0x08，與 FreeGLUT 相反 → 攔截後送出正確的字元

namespace paint::platform::keyboard {

// 安裝事件攔截器
void install();

// 視窗銷毀時呼叫：忘記在這個視窗按住中的修飾鍵
void forgetWindow(int window);

}  // namespace paint::platform::keyboard
