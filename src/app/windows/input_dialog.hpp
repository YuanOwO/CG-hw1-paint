#pragma once

#include <functional>
#include <optional>
#include <string>

#include "ui/window.hpp"

namespace paint::ui {

class InputElement;
class TextElement;

}

namespace paint::app {

class InputDialogWindow : public ui::Window {
   public:
    using SubmitCallback = std::function<void(const std::string&)>;
    using CancelCallback = std::function<void()>;
    using Validator = std::function<std::optional<std::string>(const std::string&)>;

    InputDialogWindow(const std::string& title, const std::string& message,
                      const std::string& initialValue, SubmitCallback onSubmit,
                      CancelCallback onCancel = nullptr, Validator validator = nullptr);

   private:
    std::string _message;
    SubmitCallback _onSubmit;
    CancelCallback _onCancel;
    Validator _validator;

    ui::InputElement* _input = nullptr;
    ui::TextElement* _errorText = nullptr;
    bool _finished = false;

    void submit();
    void cancel();
    void setupContent(const std::string& initialValue);
};

}  // namespace paint::app
