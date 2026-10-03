#pragma once

#include <functional>
#include <string>

#include "common/font.hpp"
#include "render/ui/button_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

struct ButtonStyle {
    ColorRGBA background = Color::White;
    ColorRGBA hovered = ColorRGBA{0.94f, 0.95f, 0.97f};
    ColorRGBA pressed = ColorRGBA{0.87f, 0.89f, 0.93f};
    ColorRGBA border = ColorRGBA{0.80f, 0.83f, 0.88f};
};

class ButtonElement : public Element {
   public:
    using ClickHandler = std::function<void()>;

    ButtonElement(ClickHandler onClick = nullptr);

    bool isHovered() const { return _hovered; }
    bool isPressed() const { return _pressed; }
    const ButtonStyle& style() const { return _style; }
    void setStyle(const ButtonStyle& style) {
        _style = style;
        invalidateDisplay();
    }

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;
    void renderContent(RenderContext& context) override;

   private:
    ButtonRenderer _renderer;
    ClickHandler _onClick;
    ButtonStyle _style;

    bool _hovered = false;
    bool _pressed = false;
};

}  // namespace paint::ui
