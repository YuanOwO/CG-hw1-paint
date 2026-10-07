#pragma once

#include <memory>
#include <string>

#include "common/color.hpp"
#include "ui/elements.hpp"
#include "ui/theme.hpp"

namespace paint::ui {

// 對話框共用的元件建立函數，統一字型、配色與尺寸。
// margin 等版面細節由呼叫端依各視窗需求自行設定。

// 建立使用標題字型的文字元件。
std::unique_ptr<TextElement> createHeading(const std::string& text);

// 建立使用內文字型的文字元件。
std::unique_ptr<TextElement> createText(const std::string& text, const ColorRGBA& color = theme::Text);

// 建立對話框操作按鈕；primary 用於視覺上強調確認動作。
std::unique_ptr<ButtonElement> createButton(const std::string& text, ButtonElement::ClickHandler onClick,
                                            bool primary = false);

// 建立固定寬度標籤與輸入框組成的一列，並透過 input 回傳輸入框的非 owning 指標。
std::unique_ptr<StackPanelElement> createInputRow(const std::string& label, InputElement*& input, int fieldWidth,
                                                  int labelWidth = 30);

}  // namespace paint::ui
