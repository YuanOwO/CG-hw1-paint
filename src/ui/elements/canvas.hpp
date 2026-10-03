#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "app/document.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/tool.hpp"
#include "render/ui/canvas_renderer.hpp"
#include "ui/element.hpp"
#include "ui/layout/bounding.hpp"

using paint::app::Document;

namespace paint::ui {

enum class GridMode { Lines, Dots, None };

class CanvasElement : public Element {
   public:
    CanvasElement(Document& document);

    GridMode gridMode() const { return _gridMode; }
    void setGridMode(GridMode mode) {
        _gridMode = mode;
        invalidateDisplay();
    }

    const drawing::Tool currentTool() const { return _currentTool; }
    void setTool(drawing::Tool tool);

    const drawing::ShapeStyle& style() const { return _currentStyle; }
    void setStyle(const drawing::ShapeStyle& style);

    const Document& document() const { return _document; }
    const drawing::Shape* draft() const { return _activeTool ? _activeTool->preview() : nullptr; }

    void undo();
    void redo();
    void clear();

   protected:
    Size measureContent(const Size& availableSize) override;

    void renderContent(RenderContext& context) override;

   private:
    GridMode _gridMode = GridMode::Lines;

    drawing::Tool _currentTool;
    drawing::ShapeStyle _currentStyle;
    CanvasRenderer _renderer;  // 用於渲染 CanvasElement 的內容

    Document& _document;  // 參考外部的 Document，CanvasElement 不擁有 Document 的所有權

    // 目前正在使用的繪圖工具，工具存在時仍可能尚未建立草稿
    std::unique_ptr<drawing::IDrawingTool> _activeTool;

    bool isDrawing() const { return _activeTool->preview() != nullptr; }

    void resetTool();

    void handleDraftEvent(drawing::ToolEventResult result);
};

}  // namespace paint::ui
