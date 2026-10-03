#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shape_style.hpp"
#include "render/render_context.hpp"

namespace paint::drawing {
class Shape;
}

namespace paint {

class ShapeRenderer {
   public:
    void drawLineGrid(RenderContext& context, int width, int height, int spacing = 20) const;
    void drawDotGrid(RenderContext& context, int width, int height, int spacing = 20) const;
    void draw(RenderContext& context, const drawing::Shape& shape) const;
};

}  // namespace paint
