#pragma once

#include <memory>

#include "ui/element.hpp"

namespace paint::ui {

class RootElement : public Element {
   public:
    RootElement() {
        setHorizontalAlignment(Alignment::Stretch);
        setVerticalAlignment(Alignment::Stretch);
    }

    Element& setContent(std::unique_ptr<Element> content);

   protected:
    Size measureContent(const Size& availableSize) override;
    void arrangeContent(const BoundingBox& contentBounds) override;
};

}  // namespace paint::ui
