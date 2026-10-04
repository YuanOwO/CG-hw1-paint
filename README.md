# Graphtoria

Graphtoria 是一個以 C++17、OpenGL 與 FreeGLUT 實作的輕量繪圖程式，也是電腦圖學課程的作業專案。程式使用自行設計的場景、繪圖工具、UI 元件與命令歷史系統，支援基本圖形繪製、文字、復原／重做及 PPM 圖片輸出。

## 功能

- 繪圖工具：點、鉛筆、直線、矩形、圓形／橢圓、多邊形與文字
- 樣式設定：線條顏色、粗細、接合方式、端點樣式、填滿模式與填滿顏色
- 文字支援：FreeGLUT 內建字型，以及支援中文的 Cubic 11、Unifont 16 點陣字型
- 編輯功能：清空畫布、復原與重做
- 顯示輔助：線格、點格或無格線模式
- 多視窗操作
- 將畫布輸出為二進位 PPM（P6）圖片
- 關閉或開啟其他檔案前，提示尚未儲存的變更

## 環境需求

- 支援 C++17 的編譯器
- CMake 3.16 以上
- OpenGL
- GLU
- FreeGLUT
- macOS 額外需要 `pkg-config`

### macOS（Homebrew）

```bash
brew install cmake pkg-config mesa freeglut
```

### Ubuntu／Debian

```bash
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
```

Windows 可使用 MinGW-w64 或其他支援 CMake 的 C++ 工具鏈，並另外安裝 OpenGL、GLU 與 FreeGLUT 開發套件。

## 建置與執行

在專案根目錄執行：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/graphtoria
```

Windows 的執行檔通常位於 `build/graphtoria.exe`，或多組態產生器所建立的 `build/Release/graphtoria.exe`。

CMake 會在建置完成後，自動將 `assets` 目錄複製到執行檔旁；請保留該目錄，否則程式將無法載入內附字型。

## 操作方式

- 在畫布上按滑鼠右鍵開啟主選單。
- 使用滑鼠左鍵點擊或拖曳以繪圖。
- 繪製直線時按住 `Shift`，可限制為水平、垂直或 45 度方向。
- 繪製矩形或橢圓時按住 `Shift`，可建立正方形或正圓。
- 多邊形以單擊新增頂點，雙擊或按 `Enter` 完成；`Backspace` 移除上一個頂點，`Esc` 取消。
- 選擇文字工具後點擊畫布，再於對話框輸入文字。
- 視窗下方狀態列會顯示目前工具、樣式、游標位置與畫布大小。

### 快捷鍵

`Primary` 在 macOS 為 `Command`，其他平台通常為 `Ctrl`。

| 快捷鍵 | 功能 |
| --- | --- |
| `Primary + Z` | 復原 |
| `Primary + Shift + Z` | 重做 |
| `C` | 清空畫布 |
| `G` | 循環切換線格、點格與無格線 |
| `Primary + N` | 新增檔案 |
| `Primary + Shift + N` | 新增視窗 |
| `Primary + O` | 開啟檔案 |
| `Primary + S` | 儲存 |
| `Primary + Shift + S` | 另存新檔 |
| `Primary + E` | 輸出 PPM 圖片 |
| `Primary + C` | 關閉目前視窗 |
| `Primary + R` | 重新整理畫面並重設輸入狀態 |
| `1`～`6` | 鉛筆、直線、矩形、橢圓、多邊形、文字工具 |
| `[`／`]` | 將線寬與點大小減少／增加 1 px |
| `Shift + [`／`Shift + ]` | 將線寬與點大小減少／增加 5 px |

## 檔案格式

- 專案文件預設使用 `.gpt` 副檔名。
- 圖片輸出使用 `.ppm` 副檔名，格式為 PPM P6。

> [!IMPORTANT]
> `.gpt` 文件的場景序列化與反序列化目前尚未完成。現階段儲存只會建立文件框架，重新開啟後無法還原畫面內容；若要保留繪圖結果，請使用 **File → Export** 輸出 `.ppm` 圖片。

## 專案結構

```text
.
├── assets/             # 內附 GFNT 字型與授權文件
├── src/
│   ├── app/            # 應用程式、文件模型與視窗
│   ├── command/        # 命令歷史、復原／重做與檔案命令
│   ├── common/         # 顏色、座標、字型與 UTF-8 工具
│   ├── drawing/        # 場景、圖形物件、樣式與繪圖工具
│   ├── event/          # 事件型別
│   ├── input/          # 輸入狀態與快捷鍵
│   ├── io/             # 文件存取與 PPM 輸出
│   ├── render/         # 畫布、圖形、文字與 UI 渲染
│   └── ui/             # 視窗、選單、UI 元件與版面配置
└── CMakeLists.txt
```

## 已知限制

- `.gpt` 文件內容的儲存與載入尚未實作完成。
- 選取工具與選單中的自訂顏色項目目前尚未實作。
- 圖片僅能輸出為 PPM，尚未支援 PNG、JPEG 等常見格式。

## 字型授權

專案內附的 Cubic 11 與 GNU Unifont 字型，其授權條款分別收錄於：

- `assets/fonts/Cubic-11/OFL.txt`
- `assets/fonts/unifont/OFL-1.1.txt`
