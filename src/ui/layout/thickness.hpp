#pragma once

#include <stdexcept>

namespace paint {

struct Thickness {
    Thickness() : left(0), top(0), right(0), bottom(0) {}
    Thickness(int left, int top, int right, int bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    int left, top, right, bottom;

    int horizontal() const { return left + right; }

    int vertical() const { return top + bottom; }
};

struct Margin : public Thickness {
    using Thickness::Thickness;  // 繼承構造函數
};

struct Padding : public Thickness {
    Padding() : Thickness() {}
    Padding(int left, int top, int right, int bottom) : Thickness(left, top, right, bottom) {
        if (left < 0 || right < 0 || top < 0 || bottom < 0) {
            throw std::invalid_argument("Padding values cannot be negative");
        }
    }
};

}  // namespace paint
