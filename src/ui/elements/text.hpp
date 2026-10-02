#pragma once
#include <string>

#include "common/color.hpp"
#include "common/font.hpp"
#include "render/ui/text_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

class TextElement : public Element {
   public:
    TextElement(const std::string& text, const FontStyle& fontStyle, const ColorRGBA& color = Color::Black)
        : Element(), _text(text), _fontStyle(fontStyle), _color(color) {}

    const std::string& text() const { return _text; }
    void setText(const std::string& text) {
        _text = text;
        invalidateLayout();
    }

    const FontStyle& fontStyle() const { return _fontStyle; }
    void setFontStyle(const FontStyle& fontStyle) {
        _fontStyle = fontStyle;
        invalidateLayout();
    }

    const ColorRGBA& color() const { return _color; }
    void setColor(const ColorRGBA& color) {
        _color = color;
        invalidateLayout();
    }

   protected:
    Size measureContent(const Size& availableSize) override;

    void renderContent(RenderContext& context) override;

   private:
    std::string _text;
    FontStyle _fontStyle;
    ColorRGBA _color;
    TextRenderer _renderer;
};

}  // namespace paint::ui
