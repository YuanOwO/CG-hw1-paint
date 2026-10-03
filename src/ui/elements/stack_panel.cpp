#include "ui/elements/stack_panel.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace paint::ui {

Element& StackPanelElement::appendChild(std::unique_ptr<Element> child, float grow) {
    if (!std::isfinite(grow) || grow < 0.0f) {
        throw std::invalid_argument("grow must be finite and non-negative");
    }

    Element& added = Element::appendChild(std::move(child));
    _grows[&added] = grow;
    return added;
}

std::unique_ptr<Element> StackPanelElement::removeChild(Element* child) {
    auto removed = Element::removeChild(child);
    // 移除元素時也移除權重，避免留下指向已移除元素的記錄。
    _grows.erase(child);
    return removed;
}

float StackPanelElement::growOf(const Element& child) const {
    const auto it = _grows.find(&child);
    return it != _grows.end() ? it->second : 0.0f;
}

Size StackPanelElement::measureContent(const Size& availableSize) {
    // 量測只計算自然需求；grow 等到 arrange 知道實際空間後才分配。
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

    // 主軸：垂直排列時是高度，水平排列時是寬度。
    const int availableLength = vertical ? contentBounds.height : contentBounds.width;
    int desiredLength = 0;
    double totalGrow = 0.0;
    const Element* lastGrowingChild = nullptr;

    for (const auto& child : children()) {
        const Size& desired = child->desiredSize();
        // desiredSize 已包含 margin，不需要再次加入間距。
        desiredLength += vertical ? *desired.height : *desired.width;
        const float grow = growOf(*child);
        totalGrow += grow;
        if (grow > 0.0f) {
            lastGrowingChild = child.get();
        }
    }

    // 空間不足時沿用原本的排列方式，不縮小 children。
    const int remaining = std::max(0, availableLength - desiredLength);
    int allocatedExtra = 0;
    double accumulatedGrow = 0.0;

    int x = contentBounds.x;
    int y = contentBounds.y;

    for (const auto& child : children()) {
        const Size& desired = child->desiredSize();
        const float grow = growOf(*child);
        int extra = 0;

        if (remaining > 0 && grow > 0.0f) {
            accumulatedGrow += grow;
            // 依累積權重計算像素邊界，避免每項獨立取整數而遺失像素。
            // 最後一個有 grow 的元素取得剩下的像素，確保完整分配。
            const int cumulativeExtra =
                child.get() == lastGrowingChild
                    ? remaining
                    : std::clamp(static_cast<int>(std::floor(remaining * accumulatedGrow / totalGrow)),
                                 allocatedExtra, remaining);
            extra = cumulativeExtra - allocatedExtra;
            allocatedExtra = cumulativeExtra;
        }

        // 擴大的是 slot；child 的 margin 與 alignment 仍由自己的 arrange 處理。
        if (vertical) {
            const int height = *desired.height + extra;
            child->arrange({
                x,
                y,
                contentBounds.width,
                height,
            });

            y += height;
        } else {
            const int width = *desired.width + extra;
            child->arrange({
                x,
                y,
                width,
                contentBounds.height,
            });

            x += width;
        }
    }
}

}  // namespace paint::ui
