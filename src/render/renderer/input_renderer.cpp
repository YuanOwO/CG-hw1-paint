#include "render/renderer/input_renderer.hpp"

#include "ui/elements/input.hpp"

namespace paint::render {

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

void InputRenderer::renderCursor(RenderContext& context, const ui::InputElement& input) {
    if (!input.isFocused() || !input.isCursorVisible()) {
        return;
    }

    const int height = input.cursorHeight();
    if (height <= 0) {
        return;
    }

    context.pushTransform();
    context.translate(input.cursorX(), input.cursorY());
    context.fillRect(1, height, input.style().cursor);
    context.popTransform();
}

}  // namespace paint::render
