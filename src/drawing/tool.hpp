#pragma once

#include <memory>

#include "common/point.hpp"
#include "drawing/shape.hpp"
#include "input/input_state.hpp"

namespace paint::drawing {

struct ToolEventState {
    ToolEventState(const KeyboardState& kbState, const MouseState& msState)
        : keyboardState(kbState), mouseState(msState) {}

    const KeyboardState& keyboardState;  // 當前鍵盤狀態
    const MouseState& mouseState;        // 當前滑鼠狀態

    Point position() const { return mouseState.position(); }
};

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
    virtual const Shape& preview() const = 0;

    // 僅在 COMMIT 後呼叫一次；取出後由呼叫端銷毀工具。
    virtual std::unique_ptr<Shape> takeShape() = 0;

    virtual ToolEventResult onClick(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onDoubleClick(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseDown(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseUp(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseMove(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onKeyDown(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onKeyUp(ToolEventState&) { return ToolEventResult::NONE; }
};

template <typename TShape>
class DrawingTool : public IDrawingTool {
   public:
    DrawingTool(ShapeStyle style) : draft(std::make_unique<TShape>(style)) {}

    const Shape& preview() const override { return *draft; }
    std::unique_ptr<Shape> takeShape() override { return std::move(draft); }

   protected:
    std::unique_ptr<TShape> draft;
};

std::unique_ptr<IDrawingTool> createDrawingTool(Tool tool, ShapeStyle style);

}  // namespace paint::drawing
