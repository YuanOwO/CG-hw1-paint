#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shapeStyle.hpp"

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
