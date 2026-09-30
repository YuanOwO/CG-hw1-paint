#pragma once

#include "common/point.hpp"

namespace paint {

struct BoundingBox {
    BoundingBox(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    int x, y;  // 左上角座標
    int width, height;

    Point topLeft() const { return Point(x, y); }

    bool contains(const Point& point) const { return contains(point.x(), point.y()); }
    bool contains(int px, int py) const { return px >= x && px < x + width && py >= y && py < y + height; }
};

}  // namespace paint
