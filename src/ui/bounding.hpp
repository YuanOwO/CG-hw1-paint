#pragma once

namespace paint {

struct BoundingBox {
    BoundingBox(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    int x, y;  // 左上角座標
    int width, height;

    bool contains(int px, int py) const { return px >= x && px < x + width && py >= y && py < y + height; }
};

}  // namespace paint
