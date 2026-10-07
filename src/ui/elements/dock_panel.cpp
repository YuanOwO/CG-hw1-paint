#include "ui/elements/dock_panel.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace paint::ui {

Element& DockPanelElement::appendChild(std::unique_ptr<Element> child, Dock dock) {
    Element& added = Element::appendChild(std::move(child));
    _docks[&added] = dock;

    return added;
}

std::unique_ptr<Element> DockPanelElement::removeChild(Element* child) {
    auto removed = Element::removeChild(child);
    _docks.erase(child);

    return removed;
}

Dock DockPanelElement::dockOf(const Element& child) const {
    const auto it = _docks.find(&child);
    return it != _docks.end() ? it->second : Dock::Left;
}

Size DockPanelElement::measureContent(const Size& availableSize) {
    int usedWidth = 0;
    int usedHeight = 0;

    int desiredWidth = 0;
    int desiredHeight = 0;

    for (std::size_t i = 0; i < children().size(); i++) {
        auto& child = *children()[i];

        // 每個 child 只能使用前面元素留下的空間。
        Size remaining = availableSize;

        if (remaining.width.has_value()) {
            remaining.width = std::max(0, *remaining.width - usedWidth);
        }

        if (remaining.height.has_value()) {
            remaining.height = std::max(0, *remaining.height - usedHeight);
        }

        const Size desired = child.measure(remaining);
        const int width = *desired.width;
        const int height = *desired.height;

        // 需求必須容納先前占用的空間及目前的 child。
        desiredWidth = std::max(desiredWidth, usedWidth + width);

        desiredHeight = std::max(desiredHeight, usedHeight + height);

        // 最後一個 child 使用剩餘區域，不再扣除。
        if (i + 1 == children().size()) {
            break;
        }

        switch (dockOf(child)) {
        case Dock::Left:
        case Dock::Right:
            usedWidth += width;
            break;

        case Dock::Top:
        case Dock::Bottom:
            usedHeight += height;
            break;
        }
    }

    return {desiredWidth, desiredHeight};
}

void DockPanelElement::arrangeContent(const BoundingBox& contentBounds) {
    BoundingBox remaining = contentBounds;

    for (std::size_t i = 0; i < children().size(); i++) {
        auto& child = *children()[i];

        // 最後一個 child 取得所有剩餘空間。
        if (i + 1 == children().size()) {
            child.arrange(remaining);
            break;
        }

        const Size& desired = child.desiredSize();

        // 避免配置大小超出剩餘區域。
        const int width = std::min(*desired.width, remaining.width);

        const int height = std::min(*desired.height, remaining.height);

        BoundingBox slot = remaining;

        switch (dockOf(child)) {
        case Dock::Left:
            slot.width = width;

            remaining.x += width;
            remaining.width -= width;
            break;

        case Dock::Right:
            slot.x = remaining.x + remaining.width - width;
            slot.width = width;

            remaining.width -= width;
            break;

        case Dock::Top:
            slot.height = height;

            remaining.y += height;
            remaining.height -= height;
            break;

        case Dock::Bottom:
            slot.y = remaining.y + remaining.height - height;
            slot.height = height;

            remaining.height -= height;
            break;
        }

        child.arrange(slot);
    }
}

}  // namespace paint::ui
