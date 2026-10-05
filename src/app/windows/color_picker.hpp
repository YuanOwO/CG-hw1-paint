#pragma once

#include <functional>
#include <string>

#include "common/color.hpp"
#include "ui/window.hpp"

namespace paint::ui {

class ColorFieldElement;
class Element;
class HueSliderElement;
class InputElement;
class TextElement;

}  // namespace paint::ui

namespace paint::app {

// 獨立的 HSV/RGB 顏色選擇視窗。
// 視窗只負責選色，使用者按下確認後透過 callback 回傳結果，
// 不直接依賴 PaintWindow 或任何繪圖工具。
class ColorPickerWindow : public ui::Window {
   public:
    using SubmitCallback = std::function<void(const ColorRGBA&)>;
    using CancelCallback = std::function<void()>;

    ColorPickerWindow(const std::string& title, const ColorRGBA& initialColor,
                      SubmitCallback onSubmit = nullptr, CancelCallback onCancel = nullptr);

    const ColorRGBA& color() const { return _color; }

   private:
    // _color 與 _hsv 表示同一個目前顏色，分別供 RGB 輸入與 HSV 控制項使用。
    ColorRGBA _color;
    ColorHSV _hsv;
    SubmitCallback _onSubmit;
    CancelCallback _onCancel;

    // 元件由 UI tree 持有；這些非 owning 指標只用於同步其顯示狀態。
    ui::ColorFieldElement* _colorField = nullptr;
    ui::HueSliderElement* _hueSlider = nullptr;
    ui::Element* _preview = nullptr;
    ui::InputElement* _hexInput = nullptr;
    ui::InputElement* _redInput = nullptr;
    ui::InputElement* _greenInput = nullptr;
    ui::InputElement* _blueInput = nullptr;
    ui::TextElement* _errorText = nullptr;

    // 程式化更新 InputElement 時也會觸發 value changed callback，
    // 因此同步期間使用 guard 避免 RGB、Hex 與 HSV 互相遞迴更新。
    bool _updatingControls = false;

    // 防止視窗關閉事件與按鈕事件重複執行 callback。
    bool _finished = false;

    void setupContent();

    // 從 RGB 或 HSV 任一表示更新目前顏色，再同步所有控制項。
    void setColor(const ColorRGBA& color);
    void setHsv(const ColorHSV& color);
    void updateControls();

    // 處理使用者直接編輯文字欄位；格式不合法時保留原顏色並顯示錯誤。
    void updateFromHex(const std::string& value);
    void updateFromRgb();

    void submit();
    void cancel();
};

}  // namespace paint::app
