#pragma once

#include <memory>
#include <string>
#include <utility>

#include "app/document.hpp"
#include "render/color_buffer.hpp"
#include "ui/elements.hpp"
#include "ui/window.hpp"

namespace paint::app {

class GraphicsRenderWindow : public ui::Window {
   public:
    GraphicsRenderWindow(int width, int height, DocumentData documentData)
        : ui::Window("", width, height), _document(documentData) {
        auto canvas = std::make_unique<ui::CanvasElement>(_document);
        _canvas = canvas.get();
        canvas->setGridMode(ui::GridMode::None);  // 禁用網格線，因為我們只想捕捉圖像內容

        setContent(std::move(canvas));
    }

    render::ColorBuffer capture();

   private:
    ui::CanvasElement* _canvas = nullptr;
    Document _document;
};

}  // namespace paint::app
