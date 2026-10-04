#pragma once
#include <string>

#include "common/color.hpp"
#include "common/font.hpp"
#include "render/renderer/text_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

class TextElement : public Element {
   public:
    TextElement(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color = Color::Black)
        : Element(), _text(text), _fontStyle(fontStyle), _color(color) {}

    const std::string& text() const { return _text; }
    void setText(const std::string& text) {
        if (_text == text) {
            return;
        }

        _text = text;
        invalidateLayout();
    }

    const FontStyle& fontStyle() const { return _fontStyle; }
    void setFontStyle(const FontStyle& fontStyle) {
        if (_fontStyle == fontStyle) {
            return;
        }

        _fontStyle = fontStyle;
        invalidateLayout();
    }

    const ColorRGBA& color() const { return _color; }
    void setColor(const ColorRGBA& color) {
        if (_color == color) {
            return;
        }

        _color = color;
        invalidateDisplay();  // 顏色改變只需要重新渲染，不需要重新布局
    }

   protected:
    Size measureContent(const Size& availableSize) override;

    void renderContent(render::RenderContext& context) override;

   private:
    std::string _text;
    FontStyle _fontStyle;
    ColorRGBA _color;
    render::TextRenderer _renderer;
};

}  // namespace paint::ui
