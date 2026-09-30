#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "document/document.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/tool.hpp"
#include "render/renderer.hpp"
#include "ui/bounding.hpp"
#include "ui/element/element.hpp"

namespace paint {

class CanvasElement : public Element {
   public:
    CanvasElement(BoundingBox bounds, Document& document);

    void setShowGrid(bool show) {
        _showGrid = show;
        invalidate();
    }
    bool isShowGrid() const { return _showGrid; }

    const drawing::Tool currentTool() const { return _currentTool; }
    void setTool(drawing::Tool tool);

    const drawing::ShapeStyle& style() const { return _currentStyle; }
    void setStyle(const drawing::ShapeStyle& style);

    void undo();
    void redo();
    void clear();
    void newFile();

   protected:
    void renderContent() override;
    void renderContent(bool includeGrid);

    void onResize(const WindowResizeEvent& event);

    void onKeyDown(const KeyDownEvent& event);
    void onKeyUp(const KeyUpEvent& event);

    void onClick(const ClickEvent& event);
    void onDoubleClick(const DoubleClickEvent& event);
    void onMouseDown(const MouseDownEvent& event);
    void onMouseUp(const MouseUpEvent& event);
    void onMouseMove(const MouseMoveEvent& event);

   private:
    bool _showGrid;

    drawing::Tool _currentTool;
    drawing::ShapeStyle _currentStyle;
    Renderer _renderer;  // 用於繪製歷史紀錄與草稿

    Document& _document;  // 參考外部的 Document，CanvasElement 不擁有 Document 的所有權

    // 目前正在使用的繪圖工具，工具存在時仍可能尚未建立草稿
    std::unique_ptr<drawing::IDrawingTool> _activeTool;

    bool isDrawing() const { return _activeTool->preview() != nullptr; }

    void resetTool();

    void handleDraftEvent(drawing::ToolEventResult result);
};

}  // namespace paint
