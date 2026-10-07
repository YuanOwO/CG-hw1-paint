#pragma once

#include "render/render_context.hpp"

namespace paint::drawing {
class SceneObject;
}

namespace paint::render {

class SceneRenderer {
   public:
    void draw(RenderContext& context, const drawing::SceneObject& scene) const;
};

}  // namespace paint::render
