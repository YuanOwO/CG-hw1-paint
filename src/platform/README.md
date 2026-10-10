# 平台層

處理 FreeGLUT 與 Apple GLUT 的差異。Windows / Linux 使用 FreeGLUT，macOS 使用系統內建的 GLUT.framework。

## 規則

`platform/` 以外：

- 只能使用標準 GLUT，並經由 `platform/glut.hpp` 引入，不要直接引入 `<GL/freeglut.h>` 或 `<GLUT/glut.h>`。
- FreeGLUT 擴充（`freeglut_ext.h`）與兩個平台行為不同的函數，改用 `platform/platform.hpp` 提供的版本：

| GLUT                                                          | 平台層                                     |
| ------------------------------------------------------------- | ------------------------------------------ |
| `glutInit`、`glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, ...)` | `platform::initialize`                     |
| `glutMainLoop`                                                | `platform::runMainLoop`                    |
| `glutGet(GLUT_INIT_STATE)`                                    | `platform::isRunning`                      |
| `glutCreateWindow` 與各視窗的事件 callback（`glutXxxFunc`）   | `platform::createWindow` + `WindowHandler` |
| `glutDestroyWindow`                                           | `platform::destroyWindow`                  |

`glut.hpp` 在 Windows / Linux 只引入標準 GLUT（`<GL/glut.h>`），Apple GLUT 也沒有 FreeGLUT 擴充，因此誤用擴充在兩個平台上都會編譯失敗；行為不同的標準函數（例如 `glutMainLoop`）則無法由編譯器檢查，需自行注意。

## 視窗事件

`createWindow` 建立視窗時傳入一個 `WindowHandler`，之後這個視窗的事件都會呼叫它的成員函數（`onDisplay`、`onMouse` 等），參數與對應的 GLUT callback 相同。平台層記錄「視窗 ID → handler」，GLUT callback 發生時以 `glutGetWindow()` 查表後轉呼叫。

使用者關閉視窗時呼叫 `onClose`，之後平台層會銷毀視窗，不需要再呼叫 `destroyWindow`。

## Apple GLUT 與 FreeGLUT 的差異

| 差異                                                                | 處理方式                                                         |
| ------------------------------------------------------------------- | ---------------------------------------------------------------- |
| 沒有 `glutSetOption`，未註冊 `glutWMCloseFunc` 時關閉視窗會結束程式 | 每個視窗都註冊 `glutWMCloseFunc`                                 |
| 關閉視窗時只呼叫 callback，不會銷毀視窗                             | 呼叫 `onClose` 後自行銷毀                                        |
| `glutMainLoop` 不會返回                                             | `runMainLoop` 以 `glutCheckLoop` 執行，所有視窗關閉後返回        |
| 沒有 `glutGet(GLUT_INIT_STATE)`                                     | `isRunning` 自行記錄                                             |
| 先銷毀上層選單、再銷毀子選單會崩潰                                  | `Menu` 先銷毀子選單（兩個平台共用）                              |
| 沒有 `glutBitmapHeight`、`glutStrokeHeight`                         | `common/font` 自行查表（兩個平台共用）                           |
| 不會送出修飾鍵事件，也沒有 `GLUT_KEY_SHIFT_L` 等常數                | 攔截器送出 `onSpecial` / `onSpecialUp`，常數為 `platform::KEY_*` |
| Command 組合鍵會被應用程式選單攔截（例如 Cmd+S 會儲存 TIFF）        | 攔截器依實體按鍵換算字元，送出 `onKeyboard` / `onKeyboardUp`     |
| Backspace 送出 `0x7F`、Delete 送出 `0x08`，與 FreeGLUT 相反         | 攔截器送出與 FreeGLUT 相同的字元                                 |
| 右鍵選單會附加其他 App 提供的服務項目                               | 取代 Apple GLUT 彈出選單的內部方法，停用服務項目                 |

### 事件攔截器

macOS 上以 AppKit 的 local event monitor 在 Apple GLUT 之前收到鍵盤事件：

- 修飾鍵：送出 special 事件後照常交給 Apple GLUT。
- Command 組合鍵、Backspace、Delete：直接呼叫 handler，不再交給 Apple GLUT。Cmd+Q、Cmd+H、Cmd+M 保留給系統。
- 其他按鍵：照常交給 Apple GLUT。

Command 組合鍵依實體按鍵換算字元，注音等輸入法開啟時也能正確判斷；沒有 Command 的單鍵仍由 Apple GLUT 處理，輸入法開啟時會收到輸入法的字元。

### 輸入法

GLUT 的鍵盤 callback 只有一個 `unsigned char`，任何平台都無法輸入中文；注音等輸入法開啟時，單鍵快捷鍵也會失效。因此 macOS 上程式切換到前景時改用 ASCII 鍵盤配置（例如 ABC），即將切換到背景或結束時還原為原本的輸入法。

## 檔案

```text
platform/
├── glut.hpp                   依平台引入 GLUT 標頭檔
├── platform.hpp               平台層介面
├── platform_internal.hpp      平台層內部使用，供各平台的實作呼叫共用部分
├── window.cpp                 兩個平台共用：視窗事件的查表與轉呼叫（標準 GLUT）
├── freeglut/
│   └── platform.cpp           FreeGLUT 實作（Windows / Linux）
└── apple/
    ├── platform.cpp           Apple GLUT 實作（macOS）：初始化、主循環、視窗的建立與銷毀
    ├── keyboard.hpp/.cpp      事件攔截器：修飾鍵、Command 組合鍵、Backspace / Delete
    ├── input_source.hpp/.cpp  程式在前景時使用英文鍵盤配置
    ├── cocoa.hpp/.mm          Cocoa（Objective-C）：攔截鍵盤事件、NSWindow 對照、前景與背景的通知
    └── context_menu.mm        Cocoa（Objective-C）：停用右鍵選單的服務項目
```

`apple/` 下的 `.cpp` 為 C++，`.mm` 為 Objective-C++。Objective-C 的部分只負責把 macOS 的事件與視窗轉成 C++ 的資料，處理邏輯都在 C++。
