#pragma once

#include <vector>

#include "common/color.hpp"

namespace paint {

class ColorBuffer {
   public:
    void capture(int x, int y, int width, int height);
    void restore() const;

    void resize(int width, int height);

    void fill(const ColorRGBA& color);

    void invalidate() { _valid = false; }

    int width() const { return _width; }
    int height() const { return _height; }

    bool isValid() const { return _valid; }

    bool matchesSize(int width, int height) const { return _valid && _width == width && _height == height; }

   private:
    int _width = 0;
    int _height = 0;
    bool _valid = false;  // 是否有完整、可供 restore 的像素內容

    std::vector<unsigned char> _pixels;
};

}  // namespace paint
