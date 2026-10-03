#pragma once

#include "render/render_context.hpp"

namespace paint::ui {

class InputElement;

}

namespace paint {

class InputRenderer {
   public:
    void render(RenderContext& context, const ui::InputElement& input);
};

}  // namespace paint
