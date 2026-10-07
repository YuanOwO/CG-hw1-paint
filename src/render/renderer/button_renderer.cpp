#include "render/renderer/button_renderer.hpp"

#include "ui/elements.hpp"

namespace paint::render {

void ButtonRenderer::render(RenderContext& context, const ui::ButtonElement& button) {
    const auto& style = button.style();
    const auto& fill = button.isPressed()   ? style.pressed
                       : button.isHovered() ? style.hovered
                                            : style.background;

    // 先畫邊框色，再內縮 1px 疊上填充色；兩層圓角共用圓心，邊框保持 1px。
    context.fillRoundedRect(0, 0, button.width(), button.height(), 7, style.border);
    context.fillRoundedRect(1, 1, button.width() - 2, button.height() - 2, 6, fill);
}

}  // namespace paint::render
