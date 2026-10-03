#pragma once

#include <cstddef>
#include <functional>
#include <string>

#include "event/events.hpp"
#include "ui/element.hpp"

namespace paint::ui {

class TextElement;

class InputElement : public Element {
   public:
    using ValueChangedHandler = std::function<void(const std::string&)>;

    explicit InputElement(std::string value = "", std::string placeholder = "");

    const std::string& value() const { return _value; }
    void setValue(std::string value);

    const std::string& placeholder() const;
    void setPlaceholder(std::string placeholder);

    std::size_t maxLength() const { return _maxLength; }
    void setMaxLength(std::size_t maxLength);

    void setOnValueChanged(ValueChangedHandler handler);

   protected:
    Size measureContent(const Size& availableSize) override;

    void arrangeContent(const BoundingBox& contentBounds) override;

    void renderContent(RenderContext& context) override;

   private:
    std::string _value;
    std::string _placeholder;

    std::size_t _cursorPosition = 0;
    std::size_t _maxLength = 255;

    bool _focused = false;

    TextElement* _textElement = nullptr;
    ValueChangedHandler _onValueChanged;

    void handleKeyDown(KeyDownEvent& event);
    void handleTextInput(TextInputEvent& event);

    void updateDisplayedText();
    void notifyValueChanged();
};

}  // namespace paint::ui
