#include "app/windows/color_picker.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include "common/palette.hpp"
#include "event/events.hpp"
#include "ui/builders.hpp"
#include "ui/elements.hpp"
#include "ui/theme.hpp"

namespace paint::app {
namespace {

using paint::ui::Alignment;
using paint::ui::createButton;
using paint::ui::createHeading;
using paint::ui::createInputRow;
using paint::ui::createText;
using paint::ui::StackOrientation;

#pragma region Helpers

// 將 RGB 浮點色彩轉為固定長度的大寫 #RRGGBB 字串。
std::string colorToHex(const ColorRGBA& color) {
    std::ostringstream stream;
    stream << '#' << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
           << static_cast<int>(colorToByte(color.r)) << std::setw(2) << static_cast<int>(colorToByte(color.g))
           << std::setw(2) << static_cast<int>(colorToByte(color.b));
    return stream.str();
}

// 接受 #RRGGBB 或 RRGGBB；格式錯誤時不丟出例外。
std::optional<ColorRGBA> parseHexColor(const std::string& value) {
    const std::string digits = !value.empty() && value.front() == '#' ? value.substr(1) : value;
    if (digits.size() != 6 || !std::all_of(digits.begin(), digits.end(), [](unsigned char character) {
            return std::isxdigit(character) != 0;
        })) {
        return std::nullopt;
    }

    try {
        const auto packed = static_cast<unsigned long>(std::stoul(digits, nullptr, 16));
        return ColorRGBA{
            static_cast<float>((packed >> 16) & 0xFF) / 255.0f,
            static_cast<float>((packed >> 8) & 0xFF) / 255.0f,
            static_cast<float>(packed & 0xFF) / 255.0f,
        };
    } catch (...) {
        return std::nullopt;
    }
}

// 解析介於 [min, max] 的非負整數，例如 RGB 通道或 HSV 分量。
std::optional<int> parseInteger(const std::string& value, int min, int max) {
    if (value.empty() || !std::all_of(value.begin(), value.end(),
                                      [](unsigned char character) { return std::isdigit(character) != 0; })) {
        return std::nullopt;
    }

    try {
        const int number = std::stoi(value);
        if (number < min || number > max) {
            return std::nullopt;
        }
        return number;
    } catch (...) {
        return std::nullopt;
    }
}

#pragma endregion  // Helpers

}  // namespace

#pragma region Lifecycle

ColorPickerWindow::ColorPickerWindow(const std::string& title, const ColorRGBA& initialColor,
                                     SubmitCallback onSubmit, CancelCallback onCancel)
    : Window(title, 720, 456, false),
      _color(initialColor.r, initialColor.g, initialColor.b),
      _hsv(rgb2hsv(_color)),
      _onSubmit(std::move(onSubmit)),
      _onCancel(std::move(onCancel)) {
    setupContent();
    updateControls();

    // 關閉按鈕等同取消；Enter 與 Escape 提供對話框鍵盤操作。
    addEventListener<WindowCloseEvent>([this](WindowCloseEvent&) { cancel(); });
    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (event.key() == Key::Enter) {
            submit();
            event.stopPropagation();
        } else if (event.key() == Key::Escape) {
            cancel();
            event.stopPropagation();
        }
    });

    // 選色期間暫停其他視窗的輸入，行為與既有對話框一致。
    setModal(true);
}

#pragma endregion  // Lifecycle

#pragma region Content Setup

void ColorPickerWindow::setupContent() {
    // 整體採垂直排列：標題、選色主區、基本色票、操作按鈕。
    auto root = std::make_unique<ui::StackPanelElement>(StackOrientation::Vertical);
    root->setPadding({24, 24, 24, 24});
    root->setBackgroundColor(ui::theme::WindowBackground);

    // 標題
    auto heading = createHeading(title());
    heading->setMargin({0, 0, 0, 18});
    root->appendChild(std::move(heading));

    // 主選色區由飽和度 / 明度平面、Hue 滑桿及數值控制區組成。
    auto pickerRow = std::make_unique<ui::StackPanelElement>(StackOrientation::Horizontal);
    pickerRow->setMargin({0, 0, 0, 18});

    auto colorField = std::make_unique<ui::ColorFieldElement>(_hsv);
    _colorField = colorField.get();
    colorField->setMargin({0, 0, 12, 0});
    // 選色平面吃掉剩餘寬度並與右側控制區等高，讓右緣對齊按鈕、下緣對齊最後一列輸入框。
    colorField->setHorizontalAlignment(Alignment::Stretch);
    colorField->setVerticalAlignment(Alignment::Stretch);
    colorField->setOnValueChanged([this](float saturation, float value) {
        // 二維選色區不改變 Hue，只更新 Saturation 與 Value。
        setHsv({_hsv.h, saturation, value});
    });
    pickerRow->appendChild(std::move(colorField), 1.0f);

    auto hueSlider = std::make_unique<ui::HueSliderElement>(_hsv.h);
    _hueSlider = hueSlider.get();
    hueSlider->setMargin({0, 0, 24, 0});
    hueSlider->setVerticalAlignment(Alignment::Stretch);
    hueSlider->setOnValueChanged([this](float hue) { setHsv({hue, _hsv.s, _hsv.v}); });
    pickerRow->appendChild(std::move(hueSlider));

    auto controls = std::make_unique<ui::StackPanelElement>(StackOrientation::Vertical);

    auto previewLabel = createText("目前色彩", ui::theme::MutedText);
    previewLabel->setMargin({0, 0, 0, 6});
    controls->appendChild(std::move(previewLabel));

    auto preview = std::make_unique<ui::Element>();
    _preview = preview.get();
    preview->setPreferredSize({300, 48});
    preview->setMargin({0, 0, 0, 12});
    controls->appendChild(std::move(preview));

    // 輸入列之間留 4px；最後一列不留，讓下緣與選色平面對齊。
    const auto spaced = [](std::unique_ptr<ui::StackPanelElement> row) {
        row->setMargin({0, 0, 0, 4});
        return row;
    };

    controls->appendChild(spaced(createInputRow("Hex", _hexInput, 270)));
    _hexInput->setMaxLength(7);

    // RGB 與 HSV 輸入並排，兩欄同步顯示同一個顏色。
    auto channels = std::make_unique<ui::StackPanelElement>(StackOrientation::Horizontal);

    auto rgbColumn = std::make_unique<ui::StackPanelElement>(StackOrientation::Vertical);
    rgbColumn->setMargin({0, 0, 20, 0});
    rgbColumn->appendChild(spaced(createInputRow("R", _redInput, 110)));
    rgbColumn->appendChild(spaced(createInputRow("G", _greenInput, 110)));
    rgbColumn->appendChild(createInputRow("B", _blueInput, 110));
    channels->appendChild(std::move(rgbColumn));

    auto hsvColumn = std::make_unique<ui::StackPanelElement>(StackOrientation::Vertical);
    hsvColumn->appendChild(spaced(createInputRow("H", _hueInput, 110)));
    hsvColumn->appendChild(spaced(createInputRow("S", _saturationInput, 110)));
    hsvColumn->appendChild(createInputRow("V", _valueInput, 110));
    channels->appendChild(std::move(hsvColumn));

    controls->appendChild(std::move(channels));
    _redInput->setMaxLength(3);
    _greenInput->setMaxLength(3);
    _blueInput->setMaxLength(3);
    _hueInput->setMaxLength(3);
    _saturationInput->setMaxLength(3);
    _valueInput->setMaxLength(3);

    _hexInput->setOnValueChanged([this](const std::string& value) { updateFromHex(value); });
    _redInput->setOnValueChanged([this](const std::string&) { updateFromRgb(); });
    _greenInput->setOnValueChanged([this](const std::string&) { updateFromRgb(); });
    _blueInput->setOnValueChanged([this](const std::string&) { updateFromRgb(); });
    _hueInput->setOnValueChanged([this](const std::string&) { updateFromHsv(); });
    _saturationInput->setOnValueChanged([this](const std::string&) { updateFromHsv(); });
    _valueInput->setOnValueChanged([this](const std::string&) { updateFromHsv(); });

    pickerRow->appendChild(std::move(controls));
    root->appendChild(std::move(pickerRow));

    // 錯誤訊息放在選色區下方並靠右；空字串高度為 0，不會撐高選色區。
    auto errorText = createText("", ui::theme::ErrorText);
    _errorText = errorText.get();
    errorText->setHorizontalAlignment(Alignment::End);
    root->appendChild(std::move(errorText));

    // 基本色票使用共用 palette，點擊後仍透過 setColor() 同步所有欄位。
    auto swatchLabel = createText("基本色彩", ui::theme::MutedText);
    swatchLabel->setMargin({0, 0, 0, 8});
    root->appendChild(std::move(swatchLabel));

    auto swatches = std::make_unique<ui::StackPanelElement>(StackOrientation::Horizontal);
    swatches->setMargin({0, 0, 0, 18});
    for (const Color namedColor : palette::Basic) {
        const ColorRGBA color = namedColor;
        auto swatch = std::make_unique<ui::ButtonElement>([this, color]() { setColor(color); });
        swatch->setPreferredSize({36, 36});
        swatch->setPadding({0, 0, 0, 0});
        swatch->setMargin({0, 0, 6, 0});
        swatch->setFocusable(false);
        swatch->setStyle({
            color,
            {std::min(1.0f, color.r + 0.12f), std::min(1.0f, color.g + 0.12f),
              std::min(1.0f, color.b + 0.12f)},
            {color.r * 0.82f, color.g * 0.82f, color.b * 0.82f},
            {0.60f, 0.63f, 0.68f},
        });
        swatches->appendChild(std::move(swatch));
    }
    root->appendChild(std::move(swatches));

    // spacer 吸收剩餘寬度，讓操作按鈕靠右排列。
    auto buttons = std::make_unique<ui::StackPanelElement>(StackOrientation::Horizontal);
    auto cancelButton = createButton("取消", [this]() { cancel(); });
    cancelButton->setMargin({0, 0, 12, 0});
    buttons->appendChild(std::make_unique<ui::Element>(), 1.0f);
    buttons->appendChild(std::move(cancelButton));
    buttons->appendChild(createButton("確定", [this]() { submit(); }, true));
    root->appendChild(std::move(buttons));

    setContent(std::move(root));
}

#pragma endregion  // Content Setup

#pragma region Color Synchronization

void ColorPickerWindow::setColor(const ColorRGBA& color) {
    // 此選色器暫不編輯 Alpha，因此輸入顏色一律轉成不透明 RGB。
    _color = {std::clamp(color.r, 0.0f, 1.0f), std::clamp(color.g, 0.0f, 1.0f),
              std::clamp(color.b, 0.0f, 1.0f)};
    _hsv = rgb2hsv(_color);
    updateControls();
}

void ColorPickerWindow::setHsv(const ColorHSV& color) {
    // Hue 可接受任意角度；Saturation 與 Value 則限制在標準範圍內。
    _hsv = {normalizeHue(color.h), std::clamp(color.s, 0.0f, 1.0f), std::clamp(color.v, 0.0f, 1.0f)};
    _color = hsv2rgb(_hsv);
    updateControls();
}

void ColorPickerWindow::updateControls() {
    if (_updatingControls) {
        return;
    }

    // setter 可能觸發 InputElement callback，guard 必須涵蓋整批同步操作。
    _updatingControls = true;
    _colorField->setColor(_hsv);
    _hueSlider->setHue(_hsv.h);
    _preview->setBackgroundColor(_color);
    _hexInput->setValue(colorToHex(_color));
    _redInput->setValue(std::to_string(colorToByte(_color.r)));
    _greenInput->setValue(std::to_string(colorToByte(_color.g)));
    _blueInput->setValue(std::to_string(colorToByte(_color.b)));
    // HSV 以整數顯示：H 為角度，S 與 V 為百分比；四捨五入到 360 時折回 0。
    _hueInput->setValue(std::to_string(static_cast<int>(std::lround(_hsv.h)) % 360));
    _saturationInput->setValue(std::to_string(std::lround(_hsv.s * 100.0f)));
    _valueInput->setValue(std::to_string(std::lround(_hsv.v * 100.0f)));
    _errorText->setText("");
    _updatingControls = false;
}

#pragma endregion  // Color Synchronization

#pragma region Text Input

void ColorPickerWindow::updateFromHex(const std::string& value) {
    if (_updatingControls) {
        return;
    }

    const auto parsed = parseHexColor(value);
    if (!parsed) {
        // 編輯中的暫時不完整字串不應覆蓋最後一個有效顏色。
        _errorText->setText("請輸入 #RRGGBB 格式。");
        return;
    }

    setColor(*parsed);
}

void ColorPickerWindow::updateFromRgb() {
    if (_updatingControls) {
        return;
    }

    const auto red = parseInteger(_redInput->value(), 0, 255);
    const auto green = parseInteger(_greenInput->value(), 0, 255);
    const auto blue = parseInteger(_blueInput->value(), 0, 255);
    if (!red || !green || !blue) {
        // 任一欄位不合法時保留目前顏色，等待使用者完成輸入。
        _errorText->setText("RGB 數值必須介於 0 到 255。");
        return;
    }

    setColor({*red / 255.0f, *green / 255.0f, *blue / 255.0f});
}

void ColorPickerWindow::updateFromHsv() {
    if (_updatingControls) {
        return;
    }

    const auto hue = parseInteger(_hueInput->value(), 0, 360);
    const auto saturation = parseInteger(_saturationInput->value(), 0, 100);
    const auto value = parseInteger(_valueInput->value(), 0, 100);
    if (!hue || !saturation || !value) {
        // 與 RGB 相同：任一欄位不合法時保留目前顏色，等待使用者完成輸入。
        _errorText->setText("H 須介於 0 到 360，S、V 須介於 0 到 100。");
        return;
    }

    // 直接走 setHsv()，保留使用者輸入的 Hue，避免低飽和度時經 RGB 轉換而遺失。
    setHsv({static_cast<float>(*hue), *saturation / 100.0f, *value / 100.0f});
}

#pragma endregion  // Text Input

#pragma region Completion

void ColorPickerWindow::submit() {
    if (_finished) {
        return;
    }

    // 先標記完成，避免 callback 間接關閉視窗時再次提交。
    _finished = true;
    if (_onSubmit) {
        _onSubmit(_color);
    }
    close();
}

void ColorPickerWindow::cancel() {
    if (_finished) {
        return;
    }

    _finished = true;
    if (_onCancel) {
        _onCancel();
    }
    close();
}

#pragma endregion  // Completion

}  // namespace paint::app
