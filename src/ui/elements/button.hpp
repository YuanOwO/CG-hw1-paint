#pragma once

#include <functional>
#include <string>

#include "common/font.hpp"
#include "render/ui/button_renderer.hpp"
#include "ui/element.hpp"

namespace paint::ui {

class ButtonElement : public Element {
   public:
    using ClickHandler = std::function<void()>;

    ButtonElement(ClickHandler onClick = nullptr);

    bool isHovered() const { return _hovered; }
    bool isPressed() const { return _pressed; }

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;
    void renderContent(RenderContext& context) override;

   private:
    ButtonRenderer _renderer;
    ClickHandler _onClick;

    bool _hovered = false;
    bool _pressed = false;
};

}  // namespace paint::ui
