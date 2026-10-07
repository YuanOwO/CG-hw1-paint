#pragma once

#include <initializer_list>
#include <optional>
#include <stdexcept>

namespace paint {

struct Size {
    Size() : width(std::nullopt), height(std::nullopt) {}
    Size(int width, int height) : width(width), height(height) {
        if (width < 0 || height < 0) {
            throw std::invalid_argument("Size cannot be negative");
        }
    }
    Size(std::initializer_list<int> list) {
        if (list.size() != 2) {
            throw std::invalid_argument("Size initializer list must have exactly two elements");
        }
        auto it = list.begin();
        width = *it++;
        height = *it;
        if (*width < 0 || *height < 0) {
            throw std::invalid_argument("Size cannot be negative");
        }
    }

    std::optional<int> width, height;  // 使用 std::nullopt 表示大小為 auto

    bool operator==(const Size& other) const { return width == other.width && height == other.height; }
    bool operator!=(const Size& other) const { return !(*this == other); }
};

}  // namespace paint
