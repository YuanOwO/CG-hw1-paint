#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "drawing/shapeStyle.hpp"
#include "drawing/tool.hpp"
#include "element/element.hpp"

namespace paint {

class CanvasElement : public Element {
   public:
    CanvasElement();

    void setTool(drawing::Tool tool) { currentTool = tool; }
    drawing::Tool getTool() const { return currentTool; }

    void setLineWidth(int width) { currentStyle.stroke.width = std::max(width, 1); }
    int getLineWidth() const { return currentStyle.stroke.width; }

    void setColor(const ColorRGBA& color) { currentStyle.stroke.color = color; }
    ColorRGBA getColor() const { return currentStyle.stroke.color; }

    void setFillColor(const ColorRGBA& color) { currentStyle.fill.color = color; }
    ColorRGBA getFillColor() const { return currentStyle.fill.color; }

    void setLineJoin(drawing::LineJoin join) { currentStyle.stroke.join = join; }
    drawing::LineJoin getLineJoin() const { return currentStyle.stroke.join; }

    void setLineCap(drawing::LineCap cap) { currentStyle.stroke.cap = cap; }
    drawing::LineCap getLineCap() const { return currentStyle.stroke.cap; }

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
    drawing::Tool currentTool;
    drawing::ShapeStyle currentStyle;

    // 目前正在使用的繪圖工具，若為 nullptr 則表示沒有正在繪製的草稿
    std::unique_ptr<drawing::IDrawingTool> activeTool;

    std::vector<std::unique_ptr<drawing::Shape>> history;
    std::vector<std::unique_ptr<drawing::Shape>> redoStack;

    bool isDrawing() const { return activeTool != nullptr; }

    void clearDraft();

    void createDraft();

    void handleDraftEvent(drawing::ToolEventResult result);
};

}  // namespace paint
