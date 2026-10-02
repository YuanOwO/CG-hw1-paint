#pragma once

#include "common/point.hpp"

namespace paint {

struct Thickness {
    Thickness() : left(0), right(0), top(0), bottom(0) {}
    Thickness(int left, int right, int top, int bottom)
        : left(left), right(right), top(top), bottom(bottom) {}

    int left, right, top, bottom;

    int horizontal() const { return left + right; }

    int vertical() const { return top + bottom; }
};

struct Margin : public Thickness {
    using Thickness::Thickness;  // 繼承構造函數
};

struct Padding : public Thickness {
    Padding() : Thickness() {}
    Padding(int left, int right, int top, int bottom) : Thickness(left, right, top, bottom) {
        if (left < 0 || right < 0 || top < 0 || bottom < 0) {
            throw std::invalid_argument("Padding values cannot be negative");
        }
    }
};

}  // namespace paint
