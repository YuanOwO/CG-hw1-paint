#pragma once

#include <unordered_map>

#include "ui/element.hpp"

namespace paint::ui {

enum class StackOrientation {
    Vertical,
    Horizontal,
};

class StackPanelElement : public Element {
   public:
    StackPanelElement(StackOrientation orientation = StackOrientation::Vertical)
        : Element(), _orientation(orientation) {
        setHorizontalAlignment(Alignment::Stretch);
        setVerticalAlignment(Alignment::Stretch);
    }

    // grow 是分配主軸剩餘空間的權重；0 表示只保留原本需求大小。
    Element& appendChild(std::unique_ptr<Element> child, float grow = 0.0f);
    std::unique_ptr<Element> removeChild(Element* child);

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;

   private:
    float growOf(const Element& child) const;

    StackOrientation _orientation;
    // children 仍由 Element 持有，這裡只保存排版用的權重。
    std::unordered_map<const Element*, float> _grows;
};

}  // namespace paint::ui
