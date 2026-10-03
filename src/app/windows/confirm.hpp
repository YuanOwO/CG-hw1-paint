#pragma once

#include <functional>
#include <string>

#include "ui/window.hpp"

namespace paint::app {

class ConfirmWindow : public ui::Window {
   public:
    using Callback = std::function<void()>;

    ConfirmWindow(const std::string& title, const std::string& message, Callback onConfirm,
                  Callback onCancel = nullptr);

   private:
    std::string _message;
    Callback _onConfirm;
    Callback _onCancel;

    void confirm();
    void cancel();

    void setupContent();
};

}  // namespace paint::app
