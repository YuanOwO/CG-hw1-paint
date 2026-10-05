#include "app/windows/confirm.hpp"

#include "common/color.hpp"
#include "common/font.hpp"
#include "ui/elements/button.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/elements/text.hpp"
#include "ui/theme.hpp"

namespace paint::app {

ConfirmWindow::ConfirmWindow(const std::string& title, const std::string& message, Callback onConfirm,
                             Callback onCancel)
    : Window(title, 440, 220, false),
      _message(message),
      _onConfirm(std::move(onConfirm)),
      _onCancel(std::move(onCancel)) {
    setupContent();

    addEventListener<WindowCloseEvent>([this](WindowCloseEvent&) {
        if (_onCancel) {
            _onCancel();
        }
    });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (event.key() == Key::Escape) {
            cancel();
            event.stopPropagation();
        }
    });

    setModal(true);
}

void ConfirmWindow::confirm() {
    if (_onConfirm) {
        _onConfirm();
    }
    close();
}

void ConfirmWindow::cancel() {
    if (_onCancel) {
        _onCancel();
    }
    close();
}

void ConfirmWindow::setupContent() {
    auto root = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Vertical);

    root->setPadding({28, 28, 28, 24});
    root->setBackgroundColor(ui::theme::WindowBackground);

    auto heading = std::make_unique<ui::TextElement>(this->title(), ui::theme::HeadingFont,
                                                     ui::theme::Text);
    heading->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(heading));

    auto message =
        std::make_unique<ui::TextElement>(_message, ui::theme::BodyFont, ui::theme::MutedText);

    message->setMargin({0, 0, 0, 24});
    root->appendChild(std::move(message), 1.0f);

    // 設置按鈕區域
    auto buttons = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);

    auto cancelButton = std::make_unique<ui::ButtonElement>([this]() { cancel(); });
    cancelButton->setPreferredSize({104, 36});
    cancelButton->setMargin({0, 0, 12, 0});

    auto cancelText = std::make_unique<ui::TextElement>("取消", ui::theme::BodyFont);
    cancelText->setColor(ui::theme::ButtonText);
    cancelText->setHorizontalAlignment(ui::Alignment::Center);
    cancelText->setVerticalAlignment(ui::Alignment::Center);
    cancelButton->appendChild(std::move(cancelText));

    auto confirmButton = std::make_unique<ui::ButtonElement>([this]() { confirm(); });
    confirmButton->setPreferredSize({104, 36});
    confirmButton->setStyle({
        ui::theme::Primary,
        ui::theme::PrimaryHovered,
        ui::theme::PrimaryPressed,
        ui::theme::Primary,
    });

    auto confirmText = std::make_unique<ui::TextElement>("確認", ui::theme::BodyFont);
    confirmText->setColor(Color::White);
    confirmText->setHorizontalAlignment(ui::Alignment::Center);
    confirmText->setVerticalAlignment(ui::Alignment::Center);
    confirmButton->appendChild(std::move(confirmText));

    buttons->appendChild(std::make_unique<ui::Element>(), 1.0f);  // 左側 spacer
    buttons->appendChild(std::move(cancelButton));
    buttons->appendChild(std::move(confirmButton));

    root->appendChild(std::move(buttons));

    setContent(std::move(root));
}

}  // namespace paint::app
