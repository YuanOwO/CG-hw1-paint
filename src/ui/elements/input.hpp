#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "common/color.hpp"
#include "event/events.hpp"
#include "render/renderer/input_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

class TextElement;

// 輸入框的最小編輯單位。之後若要快取 glyph advance 或排版位置，
// 可以直接擴充這個 struct，不需要改變文字編輯介面。
struct InputCharacter {
    char32_t codepoint;
};

struct InputStyle {
    ColorRGBA background = Color::White;
    ColorRGBA border = ColorRGBA{0.68f, 0.71f, 0.77f};
    ColorRGBA focusedBorder = ColorRGBA{0.15f, 0.36f, 0.85f};
    ColorRGBA cursor = ColorRGBA{0.12f, 0.16f, 0.23f};
};

class InputElement : public Element {
   public:
    using ValueChangedHandler = std::function<void(const std::string&)>;

    explicit InputElement(std::string value = "", std::string placeholder = "");

    const std::string& value() const { return _value; }
    void setValue(std::string value);

    const std::vector<InputCharacter>& characters() const { return _characters; }
    std::size_t cursorIndex() const { return _cursorIndex; }

    const std::string& placeholder() const;
    void setPlaceholder(std::string placeholder);

    std::size_t maxLength() const { return _maxLength; }
    void setMaxLength(std::size_t maxLength);

    bool isFocused() const { return _focused; }
    bool isCursorVisible() const { return _cursorVisible; }

    float cursorX() const;
    float cursorY() const;
    int cursorHeight() const;

    const InputStyle& style() const { return _style; }
    void setStyle(const InputStyle& style) {
        _style = style;
        invalidateDisplay();
    }

    void setOnValueChanged(ValueChangedHandler handler);

   protected:
    Size measureContent(const Size& availableSize) override;

    void arrangeContent(const BoundingBox& contentBounds) override;

    void renderContent(render::RenderContext& context) override;
    void renderOverlay(render::RenderContext& context) override;
    void update(std::chrono::milliseconds delta) override;

   private:
    // _characters 是編輯狀態的真實來源；_value 是給既有 API 使用的 UTF-8 快取。
    std::vector<InputCharacter> _characters;
    std::string _value;
    std::string _placeholder;

    // Cursor 指向字元之間的插入位置，範圍為 [0, _characters.size()]。
    std::size_t _cursorIndex = 0;
    std::size_t _maxLength = 255;

    bool _focused = false;
    bool _cursorVisible = false;
    std::chrono::milliseconds _cursorElapsed{0};

    static constexpr std::chrono::milliseconds CURSOR_BLINK_INTERVAL{500};

    // 反引號會開啟四位十六進位組字，例如 `4f60 -> 你。
    bool _unicodeInputActive = false;
    std::string _unicodeDigits;

    render::InputRenderer _renderer;
    InputStyle _style;
    TextElement* _textElement = nullptr;
    ValueChangedHandler _onValueChanged;

    void handleKeyDown(KeyDownEvent& event);
    void handleTextInput(TextInputEvent& event);
    void commitUnicodeInput();
    void resetCursorBlink();

    void rebuildValue();
    void updateDisplayedText();
    void notifyValueChanged();
};

}  // namespace paint::ui
