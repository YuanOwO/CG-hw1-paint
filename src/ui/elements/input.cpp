#include "ui/elements/input.hpp"

#include <algorithm>
#include <memory>
#include <utility>

#include "common/color.hpp"
#include "common/font.hpp"
#include "common/utf8.hpp"
#include "ui/elements/text.hpp"

namespace paint::ui {

namespace {

bool isHexDigit(char character) {
    return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') ||
           (character >= 'A' && character <= 'F');
}

unsigned int hexDigitValue(char character) {
    if (character >= '0' && character <= '9') {
        return static_cast<unsigned int>(character - '0');
    }
    if (character >= 'a' && character <= 'f') {
        return static_cast<unsigned int>(character - 'a' + 10);
    }
    return static_cast<unsigned int>(character - 'A' + 10);
}

}  // namespace

InputElement::InputElement(std::string value, std::string placeholder)
    : _placeholder(std::move(placeholder)) {
    setValue(std::move(value));

    setFocusable(true);
    setPadding({8, 4, 8, 4});
    setPreferredSize({180, 34});

    auto text = std::make_unique<TextElement>("", GfntFontStyle{GfntFontId::CUBIC_11});
    _textElement = text.get();

    _textElement->setVerticalAlignment(Alignment::Center);
    appendChild(std::move(text));

    addEventListener<FocusEvent>([this](FocusEvent&) {
        _focused = true;
        resetCursorBlink();
        updateDisplayedText();
    });

    addEventListener<BlurEvent>([this](BlurEvent&) {
        _focused = false;
        _cursorVisible = false;
        _cursorElapsed = std::chrono::milliseconds{0};
        _unicodeInputActive = false;
        _unicodeDigits.clear();
        updateDisplayedText();
        invalidateDisplay();
    });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) { handleKeyDown(event); });

    addEventListener<TextInputEvent>([this](TextInputEvent& event) { handleTextInput(event); });

    updateDisplayedText();
}

void InputElement::setValue(std::string value) {
    // 對外仍接受 UTF-8，進入編輯模型後則統一以 code point 儲存。
    // decode 時也會把不合法的 sequence 正規化成 U+FFFD。
    std::vector<InputCharacter> characters;
    const std::u32string codepoints = utf8::toUtf32(value);
    const std::size_t count = std::min(codepoints.size(), _maxLength);
    characters.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        characters.push_back({codepoints[i]});
    }

    std::u32string normalized;
    normalized.reserve(characters.size());
    for (const auto& character : characters) {
        normalized.push_back(character.codepoint);
    }

    const std::string encoded = utf8::fromUtf32(normalized);
    if (_value == encoded) {
        return;
    }

    _characters = std::move(characters);
    _value = encoded;
    _cursorIndex = _characters.size();
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

    if (_characters.size() <= _maxLength) {
        return;
    }

    _characters.resize(_maxLength);
    _cursorIndex = std::min(_cursorIndex, _characters.size());
    rebuildValue();
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

void InputElement::renderContent(render::RenderContext& context) {
    _renderer.render(context, *this);
}

void InputElement::renderOverlay(render::RenderContext& context) {
    _renderer.renderCursor(context, *this);
}

void InputElement::update(std::chrono::milliseconds delta) {
    if (!_focused) {
        return;
    }

    _cursorElapsed += delta;
    if (_cursorElapsed < CURSOR_BLINK_INTERVAL) {
        return;
    }

    _cursorElapsed -= CURSOR_BLINK_INTERVAL;
    _cursorVisible = !_cursorVisible;
    invalidateDisplay();
}

float InputElement::cursorX() const {
    if (!_textElement) {
        return static_cast<float>(padding().left);
    }

    std::u32string prefix;
    prefix.reserve(_cursorIndex + _unicodeDigits.size() + 1);

    for (std::size_t i = 0; i < _cursorIndex; ++i) {
        prefix.push_back(_characters[i].codepoint);
    }

    if (_unicodeInputActive) {
        prefix.push_back(U'`');
        for (char digit : _unicodeDigits) {
            prefix.push_back(static_cast<unsigned char>(digit));
        }
    }

    return static_cast<float>(_textElement->x()) +
           getFontWidth(_textElement->fontStyle(), utf8::fromUtf32(prefix));
}

float InputElement::cursorY() const {
    return _textElement ? static_cast<float>(_textElement->y())
                        : static_cast<float>(padding().top);
}

int InputElement::cursorHeight() const {
    return _textElement ? _textElement->height() : 0;
}

void InputElement::handleKeyDown(KeyDownEvent& event) {
    if (!isEnabled()) {
        return;
    }

    if (_unicodeInputActive) {
        // 組字期間先處理控制鍵，避免 Backspace 誤刪已確定的文字，
        // 或 Enter 在四碼完成前就提交整個對話框。
        if (event.key() == Key::Escape) {
            _unicodeInputActive = false;
            _unicodeDigits.clear();
            updateDisplayedText();
            event.stopPropagation();
            return;
        }

        if (event.key() == Key::Backspace) {
            if (_unicodeDigits.empty()) {
                _unicodeInputActive = false;
            } else {
                _unicodeDigits.pop_back();
            }
            updateDisplayedText();
            event.stopPropagation();
            return;
        }

        if (event.key() == Key::Enter) {
            event.stopPropagation();
            return;
        }
    }

    bool handled = true;
    bool valueChanged = false;

    switch (event.key()) {
    case Key::Backspace:
        if (_cursorIndex > 0) {
            _characters.erase(_characters.begin() + _cursorIndex - 1);
            --_cursorIndex;
            valueChanged = true;
        }
        break;

    case Key::Delete:
        if (_cursorIndex < _characters.size()) {
            _characters.erase(_characters.begin() + _cursorIndex);
            valueChanged = true;
        }
        break;

    case Key::Left:
        if (_cursorIndex > 0) {
            --_cursorIndex;
        }
        break;

    case Key::Right:
        if (_cursorIndex < _characters.size()) {
            ++_cursorIndex;
        }
        break;

    case Key::Home:
        _cursorIndex = 0;
        break;

    case Key::End:
        _cursorIndex = _characters.size();
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
        rebuildValue();
        notifyValueChanged();
    }
    event.stopPropagation();
}

void InputElement::handleTextInput(TextInputEvent& event) {
    if (!isEnabled()) {
        return;
    }

    // 文字事件在這裡完成後就不再往父元件傳遞。
    event.stopPropagation();

    if (event.text().empty()) {
        return;
    }

    if (!_unicodeInputActive && event.text() == "`") {
        // 反引號本身不放入文字，只用來開始 Unicode 組字。
        _unicodeInputActive = true;
        _unicodeDigits.clear();
        updateDisplayedText();
        return;
    }

    if (_unicodeInputActive) {
        const char character = event.text()[0];
        if (!isHexDigit(character)) {
            return;
        }

        _unicodeDigits.push_back(character);
        if (_unicodeDigits.size() == 4) {
            commitUnicodeInput();
        } else {
            updateDisplayedText();
        }
        return;
    }

    if (_characters.size() >= _maxLength) {
        return;
    }

    std::u32string codepoints = utf8::toUtf32(event.text());
    const std::size_t available = _maxLength - _characters.size();
    if (codepoints.size() > available) {
        codepoints.resize(available);
    }

    if (codepoints.empty()) {
        return;
    }

    std::vector<InputCharacter> inserted;
    inserted.reserve(codepoints.size());
    for (char32_t codepoint : codepoints) {
        inserted.push_back({codepoint});
    }

    _characters.insert(_characters.begin() + _cursorIndex, inserted.begin(), inserted.end());
    _cursorIndex += inserted.size();

    rebuildValue();
    updateDisplayedText();
    notifyValueChanged();
}

void InputElement::commitUnicodeInput() {
    char32_t codepoint = 0;
    for (char digit : _unicodeDigits) {
        codepoint = (codepoint << 4) | hexDigitValue(digit);
    }

    // 透過 UTF-8 codec 正規化一次，surrogate 等無效值會變成 U+FFFD。
    const std::u32string normalized = utf8::toUtf32(utf8::encodeOne(codepoint));
    if (_characters.size() < _maxLength && !normalized.empty()) {
        _characters.insert(_characters.begin() + _cursorIndex, InputCharacter{normalized.front()});
        ++_cursorIndex;
        rebuildValue();
        notifyValueChanged();
    }

    _unicodeInputActive = false;
    _unicodeDigits.clear();
    updateDisplayedText();
}

void InputElement::resetCursorBlink() {
    _cursorVisible = _focused;
    _cursorElapsed = std::chrono::milliseconds{0};
    invalidateDisplay();
}

void InputElement::rebuildValue() {
    // 只在編輯狀態改變後重建，讓 value() 仍可回傳穩定的 const reference。
    std::u32string codepoints;
    codepoints.reserve(_characters.size());
    for (const auto& character : _characters) {
        codepoints.push_back(character.codepoint);
    }
    _value = utf8::fromUtf32(codepoints);
}

void InputElement::updateDisplayedText() {
    if (!_textElement) {
        return;
    }

    if (_focused) {
        resetCursorBlink();
    }

    if (_value.empty() && !_focused) {
        _textElement->setText(_placeholder);
        _textElement->setColor(ColorRGBA{0.52f, 0.56f, 0.63f});
        return;
    }

    std::u32string displayed;
    displayed.reserve(_characters.size() + _unicodeDigits.size() + 1);

    for (std::size_t i = 0; i <= _characters.size(); ++i) {
        if (_unicodeInputActive && i == _cursorIndex) {
            displayed.push_back(U'`');
            for (char digit : _unicodeDigits) {
                displayed.push_back(static_cast<unsigned char>(digit));
            }
        }

        if (i < _characters.size()) {
            displayed.push_back(_characters[i].codepoint);
        }
    }

    const std::string encoded = utf8::fromUtf32(displayed);
    _textElement->setText(encoded);
    _textElement->setColor(ColorRGBA{0.12f, 0.16f, 0.23f});
}

void InputElement::notifyValueChanged() {
    if (_onValueChanged) {
        _onValueChanged(_value);
    }
}

}  // namespace paint::ui
