#include "render/renderer/scene_renderer.hpp"

#include "drawing/scene_object.hpp"
#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"
#include "render/renderer/shape_renderer.hpp"
#include "render/renderer/text_renderer.hpp"

namespace paint::render {

void SceneRenderer::draw(RenderContext& context, const drawing::SceneObject& scene) const {
    if (auto* text = dynamic_cast<const drawing::TextObject*>(&scene)) {
        TextRenderer textRenderer;
        textRenderer.draw(context, *text);
    } else if (auto* shape = dynamic_cast<const drawing::ShapeObject*>(&scene)) {
        ShapeRenderer shapeRenderer;
        shapeRenderer.draw(context, *shape);
    }
}

}  // namespace paint::render
