#include "ui/elements/text.hpp"

#include <cmath>

namespace paint::ui {

Size TextElement::measureContent(const Size& availableSize) {
    // 使用 getFontWidth 和 getFontHeight 計算文字的需求大小
    float width = getFontWidth(_fontStyle, _text);
    float height = getFontHeight(_fontStyle, _text);

    return Size(static_cast<int>(std::ceil(width)), static_cast<int>(std::ceil(height)));
}

void TextElement::renderContent(render::RenderContext& context) {
    // 使用 TextRenderer 來渲染文字
    _renderer.render(context, *this);
}

}  // namespace paint::ui
