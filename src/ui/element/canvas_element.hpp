#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "document/document.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/tool.hpp"
#include "render/ui/canvas_renderer.hpp"
#include "ui/bounding.hpp"
#include "ui/element/element.hpp"

namespace paint {

class CanvasElement : public Element {
   public:
    CanvasElement(BoundingBox bounds, Document& document);

    bool isGridVisible() const { return _gridVisible; }
    void setGridVisibility(bool visible) {
        _gridVisible = visible;
        invalidate();
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
    void newFile();

   protected:
    void renderContent(RenderContext& context) override;
    void renderContent(RenderContext& context, bool includeGrid);

   private:
    bool _gridVisible;

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

}  // namespace paint
