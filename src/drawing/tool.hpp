#pragma once

#include <memory>

#include "common/point.hpp"
#include "drawing/shape.hpp"
#include "event/event.hpp"
#include "input/input_state.hpp"

namespace paint::drawing {

enum class Tool {
    TOOL_PENCIL,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_ELLIPSE,
    TOOL_POLYGON,
};

enum class ToolEventResult {
    NONE,    // 表示不需要提交草稿，繼續繪製
    COMMIT,  // 表示草稿已完成，提交草稿
    CANCEL,  // 表示草稿已取消，清除草稿但不提交
};

// 所有繪圖工具的共同介面，供畫布統一操作。
class IDrawingTool {
   public:
    virtual ~IDrawingTool() = default;

    // 返回草稿的參考，供畫布在 display 時繪製。
    virtual const Shape* preview() const = 0;

    // 僅在 COMMIT 後呼叫一次；取出後由呼叫端銷毀工具。
    virtual std::unique_ptr<Shape> takeShape() = 0;

    virtual ToolEventResult onKeyDown(const KeyboardEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onKeyUp(const KeyboardEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onClick(const MouseClickEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onDoubleClick(const MouseClickEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseDown(const MouseEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseUp(const MouseEvent& event) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseMove(const MouseMoveEvent& event) { return ToolEventResult::NONE; }
};

template <typename TShape>
class DrawingTool : public IDrawingTool {
   public:
    DrawingTool(ShapeStyle style) : draft(std::make_unique<TShape>(style)) {}

    const Shape* preview() const override { return draft.get(); }
    std::unique_ptr<Shape> takeShape() override { return std::move(draft); }

   protected:
    std::unique_ptr<TShape> draft;
};

std::unique_ptr<IDrawingTool> createDrawingTool(Tool tool, ShapeStyle style);

}  // namespace paint::drawing
