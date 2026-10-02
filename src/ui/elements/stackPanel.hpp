#pragma once

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

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;

   private:
    StackOrientation _orientation;
};

}  // namespace paint::ui
