#include "render/ui/input_renderer.hpp"

#include "ui/elements/input.hpp"

namespace paint {

void InputRenderer::render(RenderContext& context, const ui::InputElement& input) {
    const auto& style = input.style();
    const ColorRGBA& border = input.isFocused() ? style.focusedBorder : style.border;

    context.fillRect(input.width(), input.height(), border);

    if (input.width() <= 2 || input.height() <= 2) {
        return;
    }

    context.pushTransform();
    context.translate(1.0f, 1.0f);
    context.fillRect(input.width() - 2, input.height() - 2, style.background);
    context.popTransform();
}

}  // namespace paint
