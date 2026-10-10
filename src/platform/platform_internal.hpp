#pragma once

#include "platform/platform.hpp"

namespace paint::platform {

// 以下由 window.cpp 提供，給各平台的實作使用

// 記住視窗的 handler，並向 GLUT 註冊轉呼叫的 callback。呼叫時 window 必須是目前視窗。
void registerWindow(int window, WindowHandler& handler);

// 忘記視窗的 handler
void unregisterWindow(int window);

// 已記住的視窗 handler，找不到時回傳 nullptr
WindowHandler* findWindowHandler(int window);

// 使用者關閉視窗時呼叫：忘記 handler，再呼叫它的 onClose
void notifyClose(int window);

}  // namespace paint::platform
