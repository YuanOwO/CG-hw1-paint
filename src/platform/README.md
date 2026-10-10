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

| 差異                                                                | 狀態                                                                     |
| ------------------------------------------------------------------- | ------------------------------------------------------------------------ |
| 沒有 `glutSetOption`，未註冊 `glutWMCloseFunc` 時關閉視窗會結束程式 | 已處理：每個視窗都註冊 `glutWMCloseFunc`                                 |
| 關閉視窗時只呼叫 callback，不會銷毀視窗                             | 已處理：呼叫 `onClose` 後自行銷毀                                        |
| `glutMainLoop` 不會返回                                             | 已處理：`runMainLoop` 以 `glutCheckLoop` 執行，所有視窗關閉後返回        |
| 沒有 `glutGet(GLUT_INIT_STATE)`                                     | 已處理：`isRunning` 自行記錄                                             |
| 先銷毀上層選單、再銷毀子選單會崩潰                                  | 已處理：`Menu` 先銷毀子選單（兩個平台共用）                              |
| 沒有 `glutBitmapHeight`、`glutStrokeHeight`                         | 已處理：`common/font` 自行查表（兩個平台共用）                           |
| 不會送出修飾鍵事件，也沒有 `GLUT_KEY_SHIFT_L` 等常數                | 未處理：`input_types.cpp` 的修飾鍵暫時停用，**兩個平台目前都沒有修飾鍵** |
| Command 組合鍵會被應用程式選單攔截（例如 Cmd+S 會儲存 TIFF）        | 未處理                                                                   |
| Backspace 送出 `0x7F`、Delete 送出 `0x08`，與 FreeGLUT 相反         | 未處理                                                                   |
| 右鍵選單會附加其他 App 提供的服務項目                               | 未處理                                                                   |

未處理的項目需要 AppKit（Objective-C），預計以事件攔截器處理。

## 檔案

| 檔案                    | 內容                                              |
| ----------------------- | ------------------------------------------------- |
| `glut.hpp`              | 依平台引入 GLUT 標頭檔                            |
| `platform.hpp`          | 平台層介面                                        |
| `platform_internal.hpp` | 平台層內部使用，供各平台的實作呼叫共用部分        |
| `platform_window.cpp`   | 兩個平台共用：視窗事件的查表與轉呼叫（標準 GLUT） |
| `platform_freeglut.cpp` | FreeGLUT 實作                                     |
| `platform_apple.cpp`    | Apple GLUT 實作（macOS）                          |
