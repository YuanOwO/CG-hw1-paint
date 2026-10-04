#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "app/document.hpp"
#include "drawing/shape_style.hpp"
#include "drawing/tools/tool_factory.hpp"
#include "render/renderer/canvas_renderer.hpp"
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

    drawing::ToolKind currentTool() const { return _currentTool; }
    void setTool(drawing::ToolKind tool);

    const drawing::ShapeStyle& style() const { return _currentStyle; }
    void setStyle(const drawing::ShapeStyle& style);

    const Document& document() const { return _document; }
    drawing::ToolOverlay toolOverlay() const { return _activeTool ? _activeTool->overlay() : drawing::ToolOverlay{}; }

    void undo();
    void redo();
    void clear();

   protected:
    Size measureContent(const Size& availableSize) override;

    void renderContent(render::RenderContext& context) override;

   private:
    GridMode _gridMode = GridMode::Lines;

    drawing::ToolKind _currentTool;
    drawing::ShapeStyle _currentStyle;
    render::CanvasRenderer _renderer;  // 用於渲染 CanvasElement 的內容

    Document& _document;  // 參考外部的 Document，CanvasElement 不擁有 Document 的所有權

    // 目前正在使用的繪圖工具，工具存在時仍可能尚未建立草稿
    std::unique_ptr<drawing::ICanvasTool> _activeTool;

    bool isInteracting() const { return _activeTool && _activeTool->isInteracting(); }

    void resetTool();

    bool handleToolResult(drawing::ToolResult result);
};

}  // namespace paint::ui
