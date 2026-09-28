#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shape_style.hpp"

namespace paint::drawing {
class Shape;
}

namespace paint {

class Renderer {
   public:
    void draw(const drawing::Shape& shape) const;

   private:
    std::vector<Point> vertices;
    drawing::ShapeStyle style;
};

}  // namespace paint
