#include "app/windows/confirm.hpp"

#include <memory>
#include <utility>

#include "event/events.hpp"
#include "ui/builders.hpp"
#include "ui/elements.hpp"
#include "ui/theme.hpp"

namespace paint::app {

ConfirmWindow::ConfirmWindow(const std::string& title, const std::string& message, Callback onConfirm,
                             Callback onCancel)
    : Window(title, 440, 240, false),
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

    root->setPadding({24, 24, 24, 24});
    root->setBackgroundColor(ui::theme::WindowBackground);

    auto heading = ui::createHeading(title());
    heading->setMargin({0, 0, 0, 12});
    root->appendChild(std::move(heading));

    auto message = ui::createText(_message, ui::theme::MutedText);

    message->setMargin({0, 0, 0, 24});
    root->appendChild(std::move(message), 1.0f);

    // 設置按鈕區域
    auto buttons = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);

    auto cancelButton = ui::createButton("取消", [this]() { cancel(); });
    cancelButton->setMargin({0, 0, 12, 0});

    auto confirmButton = ui::createButton("確認", [this]() { confirm(); }, true);

    buttons->appendChild(std::make_unique<ui::Element>(), 1.0f);  // 左側 spacer
    buttons->appendChild(std::move(cancelButton));
    buttons->appendChild(std::move(confirmButton));

    root->appendChild(std::move(buttons));

    setContent(std::move(root));
}

}  // namespace paint::app
