#include "ui/elements/root.hpp"

#include <utility>

namespace paint::ui {

Element& RootElement::setContent(std::unique_ptr<Element> content) {
    // 只能有一個子元素，如果已經有子元素，先移除它
    if (children().size() > 0) {
        removeChild(children().front().get());
    }

    appendChild(std::move(content));

    return *this;
}

Size RootElement::measureContent(const Size& availableSize) {
    if (children().empty()) {
        return {0, 0};
    }

    return children().front()->measure(availableSize);
}

void RootElement::arrangeContent(const BoundingBox& contentBounds) {
    if (children().empty()) {
        return;
    }

    children().front()->arrange(contentBounds);
}

}  // namespace paint::ui
