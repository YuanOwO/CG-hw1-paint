#include "ui/elements/stackPanel.hpp"

#include <algorithm>

namespace paint::ui {

Size StackPanelElement::measureContent(const Size& availableSize) {
    const bool vertical = _orientation == StackOrientation::Vertical;

    Size childAvailable = availableSize;

    // 堆疊方向不限制，讓 child 回報需要的大小。
    if (vertical) {
        childAvailable.height = std::nullopt;
    } else {
        childAvailable.width = std::nullopt;
    }

    int width = 0;
    int height = 0;

    for (const auto& child : children()) {
        const Size desired = child->measure(childAvailable);

        if (vertical) {
            width = std::max(width, *desired.width);
            height += *desired.height;
        } else {
            width += *desired.width;
            height = std::max(height, *desired.height);
        }
    }

    return {width, height};
}

void StackPanelElement::arrangeContent(const BoundingBox& contentBounds) {
    const bool vertical = _orientation == StackOrientation::Vertical;

    int x = contentBounds.x;
    int y = contentBounds.y;

    for (const auto& child : children()) {
        const Size& desired = child->desiredSize();

        if (vertical) {
            child->arrange({
                x,
                y,
                contentBounds.width,
                *desired.height,
            });

            y += *desired.height;
        } else {
            child->arrange({
                x,
                y,
                *desired.width,
                contentBounds.height,
            });

            x += *desired.width;
        }
    }
}

}  // namespace paint::ui
