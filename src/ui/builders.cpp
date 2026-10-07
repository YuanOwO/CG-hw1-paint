#include "ui/builders.hpp"

#include <utility>

namespace paint::ui {

std::unique_ptr<TextElement> createHeading(const std::string& text) {
    return std::make_unique<TextElement>(text, theme::HeadingFont, theme::Text);
}

std::unique_ptr<TextElement> createText(const std::string& text, const ColorRGBA& color) {
    return std::make_unique<TextElement>(text, theme::BodyFont, color);
}

std::unique_ptr<ButtonElement> createButton(const std::string& text, ButtonElement::ClickHandler onClick,
                                            bool primary) {
    auto button = std::make_unique<ButtonElement>(std::move(onClick));
    button->setPreferredSize({120, 36});

    // Enter 由對話框統一處理，避免按鈕與視窗各執行一次 callback。
    button->setFocusable(false);

    if (primary) {
        button->setStyle({
            theme::Primary,
            theme::PrimaryHovered,
            theme::PrimaryPressed,
            theme::Primary,
        });
    }

    auto label = createText(text, primary ? ColorRGBA{Color::White} : theme::ButtonText);
    label->setHorizontalAlignment(Alignment::Center);
    label->setVerticalAlignment(Alignment::Center);
    button->appendChild(std::move(label));

    return button;
}

std::unique_ptr<StackPanelElement> createInputRow(const std::string& label, InputElement*& input, int fieldWidth,
                                                  int labelWidth) {
    auto row = std::make_unique<StackPanelElement>(StackOrientation::Horizontal);

    auto labelElement = createText(label, theme::MutedText);
    labelElement->setPreferredSize({labelWidth, 36});
    labelElement->setVerticalAlignment(Alignment::Center);
    row->appendChild(std::move(labelElement));

    auto field = std::make_unique<InputElement>("");
    input = field.get();
    field->setPreferredSize({fieldWidth, 36});
    row->appendChild(std::move(field));

    return row;
}

}  // namespace paint::ui
