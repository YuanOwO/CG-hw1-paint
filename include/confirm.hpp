#pragma once

namespace confirm {

// 確認期間，呼叫端可用此狀態暫停主視窗的修改操作。
bool isOpen();

void showConfirmationWindow(const char* windowTitle, const char* message, void (*onConfirm)(),
                            void (*onCancel)());

}  // namespace confirm
