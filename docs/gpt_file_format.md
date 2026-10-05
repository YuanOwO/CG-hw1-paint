# Graphtoria `.gpt` 文件格式

本文件定義 Graphtoria 專案文件格式第 1 版。格式目標是：

- 完整還原 `DocumentData` 中的畫布與 `Scene`。
- 保留場景物件順序，也就是由底層到頂層的繪製順序。
- 不依賴第三方序列化函式庫。
- 便於人工檢查，並能可靠保存 UTF-8、多行文字與空白。
- 對損毀或不支援的檔案明確失敗，不載入半份文件。

## 基本編碼

- 檔案副檔名為 `.gpt`。
- 除 `TEXT` 的內容外，所有記錄都是以 LF（`\n`）結尾的 ASCII 行。
- 文字內容使用 UTF-8。讀取器可同時接受 LF 與 CRLF 記錄行，但寫入器一律輸出 LF。
- 關鍵字與 symbolic value 區分大小寫，固定使用本文件列出的形式。
- 整數使用十進位；浮點數必須是有限的十進位數，不允許 `nan` 或 `inf`。
- 寫入浮點數時使用 classic/C locale 以及足以 round-trip `float` 的精度
  （`std::numeric_limits<float>::max_digits10`）。
- 記錄之間不得插入未定義的行；v1 讀取器採嚴格解析。

## 文件結構

```text
GRAPHTORIA 1
CANVAS <width> <height>
OBJECTS <count>
<object 0>
<object 1>
...
END
```

`width` 與 `height` 是非負整數。`0 0` 表示畫布尺寸尚未設定。`count` 必須等於
後續的物件數量。物件依出現順序加入 `Scene`，第一個物件在最底層，最後一個物件
在最上層。

文件不保存檔名、Undo/Redo 歷史、目前工具、選取狀態、格線模式或視窗狀態；這些
不是繪圖內容，或可由執行階段重新建立。

## 圖形物件

每個圖形物件使用以下外框：

```text
OBJECT <type>
SHAPE_STYLE <fill-mode> <point-size>
FILL <r> <g> <b> <a>
STROKE <width> <r> <g> <b> <a> <join> <cap> <miter-limit>
<geometry>
END_OBJECT
```

允許的 symbolic value：

| 欄位 | 值 |
| --- | --- |
| `type` | `point`, `line`, `rectangle`, `ellipse`, `path`, `polygon` |
| `fill-mode` | `outline`, `filled`, `advanced` |
| `join` | `none`, `miter`, `bevel`, `round` |
| `cap` | `butt`, `square`, `round` |

顏色分量 `r g b a` 的範圍都是 0 到 1。`point-size`、stroke `width` 與
`miter-limit` 至少為 1。即使某個形狀目前不會使用所有 style 欄位，寫入器仍須完整
輸出，以確保載入後的物件資料相同。

各種 geometry 記錄如下：

```text
# point
POSITION <x> <y>

# line、rectangle、ellipse
BOUNDS <start-x> <start-y> <end-x> <end-y>

# path、polygon
POINTS <count>
POINT <x0> <y0>
POINT <x1> <y1>
...
```

`POINTS` 的數量必須與後續 `POINT` 行數相同。`path` 可以有零個點；`polygon`
也可保存尚未構成封閉區域的少於三個點，讓格式忠實表示模型，而不是替模型猜測或
修正內容。

## 文字物件

```text
OBJECT text
POSITION <x> <y>
TEXT_STYLE <r> <g> <b> <a> <line-spacing>
FONT <font-kind> <font-name> [<size>]
TEXT <byte-count>
<exactly byte-count bytes of UTF-8 data>
END_OBJECT
```

`TEXT` 的 payload 後必須緊接一個 LF delimiter；delimiter 不計入 `byte-count`。
讀取器必須按 byte 數讀取，不能以 `getline` 讀取 payload。這使空字串、前後空白、
中文字與多行文字都不需要跳脫。例如字串 `Hello\n世界` 的 UTF-8 長度是 12 bytes：

```text
TEXT 12
Hello
世界
END_OBJECT
```

支援的字型如下：

| `font-kind` | `font-name` | 額外欄位 |
| --- | --- | --- |
| `bitmap` | `8x13`, `9x15`, `helvetica-10`, `helvetica-12`, `helvetica-18`, `times-roman-10`, `times-roman-24` | 無 |
| `stroke` | `roman`, `mono-roman` | 正數 `size` |
| `gfnt` | `cubic-11`, `unifont-16` | 無 |

`line-spacing` 必須是正數。字型以名稱保存，不直接保存 enum 的整數值，避免 enum
重新排序後誤讀舊檔。

## 完整範例

```text
GRAPHTORIA 1
CANVAS 800 600
OBJECTS 3
OBJECT rectangle
SHAPE_STYLE advanced 1
FILL 0.25 0.5 0.75 1
STROKE 3 0 0 0 1 miter round 4
BOUNDS 20 30 220 130
END_OBJECT
OBJECT path
SHAPE_STYLE outline 1
FILL 0 0 0 0
STROKE 5 1 0.2 0.1 1 round round 4
POINTS 3
POINT 50 200
POINT 80.5 230
POINT 140 205
END_OBJECT
OBJECT text
POSITION 40 280
TEXT_STYLE 0 0 0 1 1
FONT gfnt cubic-11
TEXT 12
Hello
世界
END_OBJECT
END
```

## 讀取與驗證規則

讀取器先建立暫時的 `DocumentData`，完成下列所有檢查後才交給目前的 `Document`：

1. 檔頭必須完全符合 `GRAPHTORIA 1`；未知 major version 回報不支援。
2. 每個必要記錄必須存在且順序正確，數量不得為負或溢位。
3. 所有座標與樣式浮點數必須有限；顏色與樣式值必須在合法範圍內。
4. `OBJECTS`、`POINTS` 與 `TEXT` 的宣告長度必須和實際內容一致。
5. `TEXT` payload 必須是合法 UTF-8，且不可被檔案結尾截斷。
6. `END` 後只允許空白；多餘資料視為格式錯誤。
7. 為避免惡意或損毀檔案耗盡記憶體，實作應設定合理上限，例如物件一百萬個、
   單一 path/polygon 一千萬個點、文字 64 MiB；超過上限直接拒絕。

錯誤訊息應包含檔名、行號（若適用）和預期的記錄，方便使用者定位問題。任何解析
失敗都不得修改目前已開啟的文件。

## 寫入可靠性

存檔時應先在目標檔案所在目錄寫入暫存檔，確認 flush/close 成功後再以 rename 取代
目標檔，避免程式中止時留下半份文件。若平台無法直接覆蓋既有檔案，應使用該平台
提供的原子替換方式；替換失敗時保留原檔並回報錯誤。

## 版本演進

`GRAPHTORIA 1` 的數字是格式 major version。任何會讓 v1 讀取器誤解資料的變更都要
升版；新版讀取器可提供明確的 v1-to-current migration。應避免在相同版本中默默新增
記錄，因為 v1 採嚴格解析。

若未來需要嵌入圖片、外部字型或大量二進位資料，建議另設 v2 container（例如 ZIP
內含 manifest 與 resources），不要在 v1 中加入無界限的特殊語法。

## 對目前程式模型的實作注意事項

- `PathShape` 與 `PolygonShape` 可用 `getVertices()` 取得需要保存的點。
- `TwoPointShape` 應保存 `start()`、`end()`，不可保存矩形或橢圓的採樣頂點；否則無法
  還原原本的參數化物件。
- 反序列化時先依 `OBJECT` type 建立正確的 concrete class，再套用 geometry。
- `LoadCommand` 目前只把 `scene` 傳給 `Document::replaceContent()`，會丟失已讀出的
  `canvasWidth` 與 `canvasHeight`。實作載入時需讓 `replaceContent` 接受完整
  `DocumentData`，或另外傳入畫布尺寸。
- 物件型別目前靠 RTTI 區分；若之後希望減少 `dynamic_cast`，可以在 `SceneObject`
  增加穩定的 `ObjectType`，但檔案仍應保存上述名稱而不是 enum 整數。

