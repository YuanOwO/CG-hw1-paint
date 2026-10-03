#include "app/windows/confirm.hpp"

#include "common/color.hpp"
#include "common/font.hpp"
#include "ui/elements/button.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/elements/text.hpp"

namespace paint::app {

ConfirmWindow::ConfirmWindow(const std::string& title, const std::string& message, Callback onConfirm,
                             Callback onCancel)
    : Window(title, 400, 200, false),
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

    root->setPadding({20, 20, 20, 20});

    auto message =
        std::make_unique<ui::TextElement>(_message, BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18});

    message->setMargin({0, 0, 0, 24});
    root->appendChild(std::move(message), 1.0f);

    // 設置按鈕區域
    auto buttons = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);

    auto cancelButton = std::make_unique<ui::ButtonElement>([this]() { cancel(); });
    cancelButton->setPreferredSize({100, 32});
    cancelButton->setMargin({0, 0, 8, 0});

    auto cancelText =
        std::make_unique<ui::TextElement>("Cancel", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    cancelButton->appendChild(std::move(cancelText));

    auto confirmButton = std::make_unique<ui::ButtonElement>([this]() { confirm(); });
    confirmButton->setPreferredSize({100, 32});

    auto confirmText =
        std::make_unique<ui::TextElement>("Confirm", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    confirmButton->appendChild(std::move(confirmText));

    buttons->appendChild(std::make_unique<ui::Element>(), 1.0f);  // 左側 spacer
    buttons->appendChild(std::move(cancelButton));
    buttons->appendChild(std::move(confirmButton));

    root->appendChild(std::move(buttons));

    setContent(std::move(root));
}

}  // namespace paint::app
