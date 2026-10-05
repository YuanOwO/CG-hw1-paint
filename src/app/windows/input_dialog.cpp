#include "app/windows/input_dialog.hpp"

#include <exception>
#include <memory>
#include <utility>

#include "common/color.hpp"
#include "common/font.hpp"
#include "ui/elements/button.hpp"
#include "ui/elements/input.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/elements/text.hpp"
#include "ui/theme.hpp"

namespace paint::app {
namespace {

std::unique_ptr<ui::ButtonElement> createButton(const std::string& text,
                                                ui::ButtonElement::ClickHandler onClick,
                                                bool primary = false) {
    auto button = std::make_unique<ui::ButtonElement>(std::move(onClick));
    button->setPreferredSize({104, 36});

    // Enter 由對話框統一處理，避免按鈕與視窗各執行一次 callback。
    button->setFocusable(false);

    if (primary) {
        button->setStyle({
            ui::theme::Primary,
            ui::theme::PrimaryHovered,
            ui::theme::PrimaryPressed,
            ui::theme::Primary,
        });
    }

    auto label = std::make_unique<ui::TextElement>(text, ui::theme::BodyFont);
    label->setColor(primary ? ColorRGBA{Color::White} : ui::theme::ButtonText);
    label->setHorizontalAlignment(ui::Alignment::Center);
    label->setVerticalAlignment(ui::Alignment::Center);
    button->appendChild(std::move(label));

    return button;
}

}  // namespace

InputDialogWindow::InputDialogWindow(const std::string& title, const std::string& message,
                                     const std::string& initialValue, SubmitCallback onSubmit,
                                     CancelCallback onCancel, Validator validator)
    : Window(title, 440, 240, false),
      _message(message),
      _onSubmit(std::move(onSubmit)),
      _onCancel(std::move(onCancel)),
      _validator(std::move(validator)) {
    setupContent(initialValue);

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

    setFocusedElement(_input);
    setModal(true);
}

void InputDialogWindow::submit() {
    if (_finished) {
        return;
    }

    // 沒有輸入內容時保持對話框開啟，讓使用者繼續輸入。
    if (_input->value().empty()) {
        _errorText->setText("輸入內容不能為空。");
        setFocusedElement(_input);
        return;
    }

    if (_validator) {
        const auto error = _validator(_input->value());
        if (error) {
            _errorText->setText(*error);
            setFocusedElement(_input);
            return;
        }
    }

    try {
        if (_onSubmit) {
            _onSubmit(_input->value());
        }
    } catch (const std::exception& error) {
        _errorText->setText(error.what());
        setFocusedElement(_input);
        return;
    }

    _finished = true;
    close();
}

void InputDialogWindow::cancel() {
    if (_finished) {
        return;
    }

    _finished = true;
    if (_onCancel) {
        _onCancel();
    }
    close();
}

void InputDialogWindow::setupContent(const std::string& initialValue) {
    auto root = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Vertical);
    root->setPadding({28, 28, 28, 24});
    root->setBackgroundColor(ui::theme::WindowBackground);

    auto heading =
        std::make_unique<ui::TextElement>(title(), ui::theme::HeadingFont, ui::theme::Text);
    heading->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(heading));

    auto message =
        std::make_unique<ui::TextElement>(_message, ui::theme::BodyFont, ui::theme::MutedText);
    message->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(message));

    auto input = std::make_unique<ui::InputElement>(initialValue);
    _input = input.get();
    input->setHorizontalAlignment(ui::Alignment::Stretch);
    input->setMargin({0, 0, 0, 6});
    root->appendChild(std::move(input));

    auto errorText =
        std::make_unique<ui::TextElement>("", ui::theme::BodyFont, ui::theme::ErrorText);
    _errorText = errorText.get();
    errorText->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(errorText));

    _input->setOnValueChanged([this](const std::string&) { _errorText->setText(""); });

    auto buttons = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);

    auto cancelButton = createButton("取消", [this]() { cancel(); });
    cancelButton->setMargin({0, 0, 12, 0});

    auto confirmButton = createButton("確認", [this]() { submit(); }, true);

    buttons->appendChild(std::make_unique<ui::Element>(), 1.0f);  // 左側留白
    buttons->appendChild(std::move(cancelButton));
    buttons->appendChild(std::move(confirmButton));

    root->appendChild(std::make_unique<ui::Element>(), 1.0f);
    root->appendChild(std::move(buttons));

    setContent(std::move(root));
}

}  // namespace paint::app
