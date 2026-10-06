# Graphtoria

電腦圖學 Homework #1：A Drawing Panel。以 C++17、OpenGL 與 FreeGLUT 實作的向量繪圖程式。

![主畫面](docs/figures/ch02_main_window.png)

## 功能

- **繪圖工具**：Select、Point、Pencil、Line、Rectangle、Ellipse、Polygon、Text
- **樣式設定**：Primary / Fill Color、Stroke Width、Line Join（Miter / Bevel / Round）、Line Cap（Butt / Square / Round）、Point Size、字型
- **填色模式**：Outline、Filled、Advanced（內外分色，並以自製 Stroke Geometry 繪製轉角與端點）
- **顏色選擇器**：色彩區域、Hue slider、RGB 數值、Hex 色碼
- **編輯**：物件選取與刪除、Undo / Redo、繪製中的 Draft 即時預覽
- **文件**：儲存 / 開啟 `.gpt` 文件、輸出 PPM P6 圖片（不含 Grid 與介面元素）
- **其他**：多視窗、Modal Dialog、Grid（Lines / Dots / None）、UTF-8 文字與 GFNT 點陣字型

## 建置

需要 CMake 3.16 以上、支援 C++17 的編譯器，以及 OpenGL / GLU / FreeGLUT。

| 平台 | 相依套件 |
| --- | --- |
| macOS | `brew install cmake pkgconf freeglut mesa mesa-glu` |
| Linux（Debian / Ubuntu） | `sudo apt install cmake freeglut3-dev libglu1-mesa-dev` |
| Windows | MinGW-w64 + FreeGLUT（MinGW 會靜態連結執行檔） |

> macOS 上的 Homebrew FreeGLUT 使用 Mesa / GLX 建立 context，因此 CMake 透過 pkg-config 連結 Mesa 的 GL / GLU，而不是 Apple 的 OpenGL framework。執行時需要 XQuartz。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build build
```

```bash
./build/graphtoria
```

建置完成後，`assets/` 會自動複製到執行檔旁邊；程式啟動時從執行檔所在目錄的 `assets/fonts` 載入字型，請保持兩者放在一起。

`Debug` 組態會開啟 `-fsanitize=undefined`。

## 操作

在 Canvas 上按右鍵開啟選單，可切換工具並調整 Stroke、Fill、Point Size、Font 與 Grid。底部狀態列顯示目前工具、樣式、滑鼠座標與畫布大小。

| 工具 | 操作 |
| --- | --- |
| Select | 單擊選取最上層物件；Esc 或點擊空白處取消，Delete / Backspace 刪除 |
| Point | 單擊放置方點 |
| Pencil | 按住左鍵拖曳繪製自由曲線 |
| Line | 拖曳起點到終點；按住 Shift 限制為水平、垂直或 45° |
| Rectangle | 拖曳對角；按住 Shift 限制為正方形 |
| Ellipse | 拖曳外接矩形；按住 Shift 限制為正圓 |
| Polygon | 單擊加入頂點，雙擊或 Enter 完成，Backspace 移除上一點 |
| Text | 單擊起始位置後在對話框輸入文字 |

### 快捷鍵

Primary 在 macOS 為 Command，在 Windows / Linux 為 Ctrl。

| 快捷鍵 | 功能 |
| --- | --- |
| Primary+Z / Primary+Shift+Z | Undo / Redo（繪製中按 Undo 會先取消 Draft） |
| Primary+N / Primary+Shift+N | 新文件 / 新視窗 |
| Primary+O | 開啟文件 |
| Primary+S / Primary+Shift+S | 儲存 / 另存新檔 |
| Primary+E | 輸出 PPM 圖片 |
| Primary+R | 重新顯示畫面並重設輸入狀態 |
| C | 清空畫布 |
| G | 切換 Grid 模式 |
| 0 – 6 | Select、Pencil、Line、Rectangle、Ellipse、Polygon、Text |
| `[` / `]` | Stroke Width 與 Point Size −1 / +1 px |
| Shift+`[` / Shift+`]` | Stroke Width 與 Point Size −5 / +5 px |

## 專案結構

```text
src/
├── main.cpp
├── app/        Application、Document 與各視窗（繪圖、對話框、顏色選擇器）
├── command/    Command 與 Undo / Redo 歷史
├── common/     顏色、座標、UTF-8、字型與 GFNT 載入
├── drawing/    Scene、圖形 / 文字物件、樣式與繪圖工具
├── event/      輸入、視窗與元素事件
├── input/      輸入狀態與快捷鍵管理
├── io/         文件存取、.gpt 序列化與 PPM 輸出
├── render/     Color Buffer、Render Context 與各元件的 Renderer
└── ui/         元素樹、版面配置、選單與主題
assets/fonts/   Cubic 11 與 Unifont 的 GFNT 點陣字型
docs/           報告各章節、圖片與圖片原始檔
```

## 報告

報告主檔為 [`hw1-paint.tex`](hw1-paint.tex)，各章節位於 `docs/`，已編譯版本為 [`hw1-paint.pdf`](hw1-paint.pdf)。使用 XeLaTeX 編譯，需安裝 Noto Serif、Noto Serif / Sans CJK TC、Menlo 與 Kaiti TC 字型。

```bash
latexmk -xelatex hw1-paint.tex
```

`docs/figures-src/` 存放圖片原始檔：`.mmd` 由 `render-mermaid.sh` 透過 Mermaid CLI（`mmdc`）輸出為 PNG，其餘圖片由同目錄的 Python 腳本產生。

## 授權

`assets/fonts/` 內的字型依各自目錄中的 OFL 授權條款散布。
