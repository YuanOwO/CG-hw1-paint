#pragma once

#include <memory>
#include <unordered_map>

#include "ui/element.hpp"

namespace paint::ui {

enum class Dock {
    Left,
    Top,
    Right,
    Bottom,
};

class DockPanelElement : public Element {
   public:
    DockPanelElement() {
        setHorizontalAlignment(Alignment::Stretch);
        setVerticalAlignment(Alignment::Stretch);
    }

    Element& appendChild(std::unique_ptr<Element> child, Dock dock = Dock::Left);

    std::unique_ptr<Element> removeChild(Element* child);

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;

   private:
    Dock dockOf(const Element& child) const;

    // 元素仍由 Element::children() 持有，
    // 這裡只記錄每個 child 的停靠方向。
    std::unordered_map<const Element*, Dock> _docks;
};

}  // namespace paint::ui
