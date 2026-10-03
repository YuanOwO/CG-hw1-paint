#include "ui/elements/input.hpp"

#include <algorithm>
#include <memory>
#include <utility>

#include "common/color.hpp"
#include "common/font.hpp"
#include "ui/elements/text.hpp"

namespace paint::ui {

InputElement::InputElement(std::string value, std::string placeholder)
    : _value(std::move(value)), _placeholder(std::move(placeholder)), _cursorPosition(_value.size()) {
    if (_value.size() > _maxLength) {
        _value.resize(_maxLength);
        _cursorPosition = _value.size();
    }

    setFocusable(true);
    setPadding({8, 4, 8, 4});
    setPreferredSize({180, 34});

    auto text = std::make_unique<TextElement>("", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    _textElement = text.get();

    // TextElement 只負責顯示。讓 hit test 落在 InputElement，點擊文字時
    // InputElement 才能成為 Window 的 focused element。
    _textElement->setEnabled(false);
    _textElement->setVerticalAlignment(Alignment::Center);
    appendChild(std::move(text));

    addEventListener<FocusEvent>([this](FocusEvent&) {
        _focused = true;
        updateDisplayedText();
        invalidateDisplay();
    });

    addEventListener<BlurEvent>([this](BlurEvent&) {
        _focused = false;
        updateDisplayedText();
        invalidateDisplay();
    });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) { handleKeyDown(event); });

    addEventListener<TextInputEvent>([this](TextInputEvent& event) { handleTextInput(event); });

    updateDisplayedText();
}

void InputElement::setValue(std::string value) {
    if (value.size() > _maxLength) {
        value.resize(_maxLength);
    }

    if (_value == value) {
        return;
    }

    _value = std::move(value);
    _cursorPosition = _value.size();
    updateDisplayedText();
    notifyValueChanged();
}

const std::string& InputElement::placeholder() const {
    return _placeholder;
}

void InputElement::setPlaceholder(std::string placeholder) {
    if (_placeholder == placeholder) {
        return;
    }

    _placeholder = std::move(placeholder);
    updateDisplayedText();
}

void InputElement::setMaxLength(std::size_t maxLength) {
    _maxLength = maxLength;

    if (_value.size() <= _maxLength) {
        return;
    }

    _value.resize(_maxLength);
    _cursorPosition = std::min(_cursorPosition, _value.size());
    updateDisplayedText();
    notifyValueChanged();
}

void InputElement::setOnValueChanged(ValueChangedHandler handler) {
    _onValueChanged = std::move(handler);
}

Size InputElement::measureContent(const Size& availableSize) {
    return _textElement->measure(availableSize);
}

void InputElement::arrangeContent(const BoundingBox& contentBounds) {
    _textElement->arrange(contentBounds);
}

void InputElement::renderContent(RenderContext& context) {
    const ColorRGBA border = _focused ? ColorRGBA{0.15f, 0.36f, 0.85f} : ColorRGBA{0.68f, 0.71f, 0.77f};

    context.fillRect(width(), height(), border);

    if (width() <= 2 || height() <= 2) {
        return;
    }

    context.pushTransform();
    context.translate(1.0f, 1.0f);
    context.fillRect(width() - 2, height() - 2, Color::White);
    context.popTransform();
}

void InputElement::handleKeyDown(KeyDownEvent& event) {
    if (!isEnabled()) {
        return;
    }

    bool handled = true;
    bool valueChanged = false;

    switch (event.key()) {
    case Key::Backspace:
        if (_cursorPosition > 0) {
            _value.erase(_cursorPosition - 1, 1);
            --_cursorPosition;
            valueChanged = true;
        }
        break;

    case Key::Delete:
        if (_cursorPosition < _value.size()) {
            _value.erase(_cursorPosition, 1);
            valueChanged = true;
        }
        break;

    case Key::Left:
        if (_cursorPosition > 0) {
            --_cursorPosition;
        }
        break;

    case Key::Right:
        if (_cursorPosition < _value.size()) {
            ++_cursorPosition;
        }
        break;

    case Key::Home:
        _cursorPosition = 0;
        break;

    case Key::End:
        _cursorPosition = _value.size();
        break;

    default:
        handled = false;
        break;
    }

    if (!handled) {
        return;
    }

    updateDisplayedText();
    if (valueChanged) {
        notifyValueChanged();
    }
    event.stopPropagation();
}

void InputElement::handleTextInput(TextInputEvent& event) {
    if (!isEnabled()) {
        return;
    }

    // 文字事件只應由目前有焦點的文字輸入元件消費。
    event.stopPropagation();

    if (event.text().empty() || _value.size() >= _maxLength) {
        return;
    }

    const std::size_t available = _maxLength - _value.size();
    const std::string text = event.text().substr(0, available);

    if (text.empty()) {
        return;
    }

    _value.insert(_cursorPosition, text);
    _cursorPosition += text.size();

    updateDisplayedText();
    notifyValueChanged();
}

void InputElement::updateDisplayedText() {
    if (!_textElement) {
        return;
    }

    if (_value.empty() && !_focused) {
        _textElement->setText(_placeholder);
        _textElement->setColor(ColorRGBA{0.52f, 0.56f, 0.63f});
        return;
    }

    std::string displayed = _value;
    if (_focused) {
        displayed.insert(_cursorPosition, "|");
    }

    _textElement->setText(displayed);
    _textElement->setColor(ColorRGBA{0.12f, 0.16f, 0.23f});
}

void InputElement::notifyValueChanged() {
    if (_onValueChanged) {
        _onValueChanged(_value);
    }
}

}  // namespace paint::ui
