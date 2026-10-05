# Graphtoria 報告 TODO

## 必須補

- Section 2.1（圖 1）：加入目前版本的 Graphtoria 主畫面 screenshot，需看得到 Canvas、狀態列與右鍵 hierarchical menu。
- Section 5.2（文字結果圖）：加入 Input dialog、caret，以及至少一組英文與中文文字結果。
- Section 8.1（綜合成果圖）：用 Point、Pencil、Line、Rectangle、Ellipse、Polygon 實際畫一張示範圖，包含不同 Stroke、Fill、Join、Cap。
- Section 8.2（多視窗圖）：加入兩個 PaintWindow 與 modal dialog 的實際畫面。
- Section 6.3（PPM 圖）：附上同一份 Scene 的程式畫面與 PPM 輸出，確認 Y 軸方向正確且輸出不含 Grid/狀態列。
- 全文：依實際課程規定確認封面姓名、學號、日期與作業名稱。

## 建議補

- Section 8.3：實際 demo 後重新核對 Requirement checklist，尤其是不同平台的 window/keyboard callback 行為。
- Section 4.3：加入程式執行結果，對照 Join、Cap、miter limit 向量示意圖。
- Section 6.2：量測 cache restore 前後的 repaint 次數或時間；若不做量測，維持目前定性說明即可。
- Section 9：從 git history 補上具體 bug/commit 例子，讓問題與解法更可追溯。
- Appendix B：以 hex dump 對照一個實際 GFNT 檔案的前 16 bytes 與第一筆 18-byte glyph entry。

## 可選

- Section 8.2：錄製或截圖 Select 選取框與刪除後 Undo/Redo、Polygon Backspace/Escape，以及 Shift constraint。
- Section 10：若之後完成 `.gpt` serialization、Select 的移動／縮放／多選、Custom color 或 Scale，更新限制與 checklist。
- Appendix A：加入 StackPanel grow 與 DockPanel 剩餘空間分配的數值範例。
- 全文：在最終繳交前依老師偏好調整 screenshot 數量；Mermaid source 已保留在 `docs/figures-src/`，可用本機 Mermaid CLI 重新產生 PDF，不依賴線上 renderer。
