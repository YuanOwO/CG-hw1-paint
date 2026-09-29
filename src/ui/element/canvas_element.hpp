#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "document/document.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/tool.hpp"
#include "render/renderer.hpp"
#include "ui/element/element.hpp"

namespace paint {

class CanvasElement : public Element {
   public:
    CanvasElement(int width, int height, Document& document);

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

    void onResize(const WindowResizeEvent& event) override;

    void onKeyDown(const KeyboardEvent& event) override;
    void onKeyUp(const KeyboardEvent& event) override;

    void onClick(const MouseClickEvent& event) override;
    void onDoubleClick(const MouseClickEvent& event) override;
    void onMouseDown(const MouseEvent& event) override;
    void onMouseUp(const MouseEvent& event) override;
    void onMouseMove(const MouseMoveEvent& event) override;

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
