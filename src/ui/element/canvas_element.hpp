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
    CanvasElement(Document& document);

    void setTool(drawing::Tool tool) { _currentTool = tool; }
    drawing::Tool getTool() const { return _currentTool; }

    void setStyle(const drawing::ShapeStyle& style) { _currentStyle = style; }
    drawing::ShapeStyle& getStyle() { return _currentStyle; }

    void undo();
    void redo();
    void clear();

   protected:
    void render() override;

    void onKeyDown(const KeyboardEvent& event) override;
    void onKeyUp(const KeyboardEvent& event) override;

    void onClick(const MouseClickEvent& event) override;
    void onDoubleClick(const MouseClickEvent& event) override;
    void onMouseDown(const MouseEvent& event) override;
    void onMouseUp(const MouseEvent& event) override;
    void onMouseMove(const MouseMoveEvent& event) override;

   private:
    drawing::Tool _currentTool;
    drawing::ShapeStyle _currentStyle;
    Renderer _renderer;  // 用於繪製歷史紀錄與草稿

    Document& _document;  // 參考外部的 Document，CanvasElement 不擁有 Document 的所有權

    // 目前正在使用的繪圖工具，若為 nullptr 則表示沒有正在繪製的草稿
    std::unique_ptr<drawing::IDrawingTool> _activeTool;

    bool isDrawing() const { return _activeTool != nullptr; }

    void clearDraft();
    void createDraft();

    void handleDraftEvent(drawing::ToolEventResult result);
};

}  // namespace paint
