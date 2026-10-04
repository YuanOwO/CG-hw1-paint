#include "app/windows/confirm.hpp"

#include "common/color.hpp"
#include "common/font.hpp"
#include "ui/elements/button.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/elements/text.hpp"

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
    root->setBackgroundColor(ColorRGBA{0.97f, 0.98f, 0.99f});

    auto heading = std::make_unique<ui::TextElement>(
        this->title(), BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18}, ColorRGBA{0.12f, 0.16f, 0.23f});
    heading->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(heading));

    auto message = std::make_unique<ui::TextElement>(_message, GfntFontStyle{GfntFontId::CUBIC_11},
                                                     ColorRGBA{0.36f, 0.40f, 0.47f});

    message->setMargin({0, 0, 0, 24});
    root->appendChild(std::move(message), 1.0f);

    // 設置按鈕區域
    auto buttons = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);

    auto cancelButton = std::make_unique<ui::ButtonElement>([this]() { cancel(); });
    cancelButton->setPreferredSize({104, 36});
    cancelButton->setMargin({0, 0, 12, 0});

    auto cancelText = std::make_unique<ui::TextElement>("取消", GfntFontStyle{GfntFontId::CUBIC_11});
    cancelText->setColor(ColorRGBA{0.22f, 0.27f, 0.34f});
    cancelText->setHorizontalAlignment(ui::Alignment::Center);
    cancelText->setVerticalAlignment(ui::Alignment::Center);
    cancelButton->appendChild(std::move(cancelText));

    auto confirmButton = std::make_unique<ui::ButtonElement>([this]() { confirm(); });
    confirmButton->setPreferredSize({104, 36});
    confirmButton->setStyle({
        {0.15f, 0.36f, 0.85f},
        {0.12f, 0.31f, 0.76f},
        {0.10f, 0.25f, 0.64f},
        {0.15f, 0.36f, 0.85f},
    });

    auto confirmText = std::make_unique<ui::TextElement>("確認", GfntFontStyle{GfntFontId::CUBIC_11});
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
