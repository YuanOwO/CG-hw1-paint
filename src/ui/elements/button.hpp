#pragma once

#include <functional>
#include <string>

#include "common/font.hpp"
#include "render/renderer/button_renderer.hpp"
#include "ui/element.hpp"
#include "ui/theme.hpp"

namespace paint::ui {

struct ButtonStyle {
    ColorRGBA background = theme::ControlBackground;
    ColorRGBA hovered = theme::ControlHovered;
    ColorRGBA pressed = theme::ControlPressed;
    ColorRGBA border = theme::ControlBorder;
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
    void renderContent(render::RenderContext& context) override;

   private:
    render::ButtonRenderer _renderer;
    ClickHandler _onClick;
    ButtonStyle _style;

    bool _hovered = false;
    bool _pressed = false;
};

}  // namespace paint::ui
