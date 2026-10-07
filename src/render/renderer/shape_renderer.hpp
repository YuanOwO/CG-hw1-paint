#pragma once

#include "render/render_context.hpp"

namespace paint::drawing {
class ShapeObject;
}

namespace paint::render {

class ShapeRenderer {
   public:
    void draw(RenderContext& context, const drawing::ShapeObject& shape) const;
};

}  // namespace paint::render
