#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "drawing/shapeStyle.hpp"
#include "drawing/tool.hpp"
#include "element/element.hpp"
#include "render/render.hpp"

namespace paint {

class CanvasElement : public Element {
   public:
    CanvasElement();

    void setTool(drawing::Tool tool) { _currentTool = tool; }
    drawing::Tool getTool() const { return _currentTool; }

    void setLineWidth(int width) { _currentStyle.stroke.width = std::max(width, 1); }
    int getLineWidth() const { return _currentStyle.stroke.width; }

    void setColor(const ColorRGBA& color) { _currentStyle.stroke.color = color; }
    ColorRGBA getColor() const { return _currentStyle.stroke.color; }

    void setFillColor(const ColorRGBA& color) { _currentStyle.fill.color = color; }
    ColorRGBA getFillColor() const { return _currentStyle.fill.color; }

    void setLineJoin(drawing::LineJoin join) { _currentStyle.stroke.join = join; }
    drawing::LineJoin getLineJoin() const { return _currentStyle.stroke.join; }

    void setLineCap(drawing::LineCap cap) { _currentStyle.stroke.cap = cap; }
    drawing::LineCap getLineCap() const { return _currentStyle.stroke.cap; }

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

    Renderer _renderer;  // 用於繪製歷史紀錄與草稿的渲染器

    // 目前正在使用的繪圖工具，若為 nullptr 則表示沒有正在繪製的草稿
    std::unique_ptr<drawing::IDrawingTool> _activeTool;

    std::vector<std::unique_ptr<drawing::Shape>> _history;
    std::vector<std::unique_ptr<drawing::Shape>> _redoStack;

    bool isDrawing() const { return _activeTool != nullptr; }

    void clearDraft();

    void createDraft();

    void handleDraftEvent(drawing::ToolEventResult result);
};

}  // namespace paint
