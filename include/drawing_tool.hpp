#pragma once

#include <memory>
#include <utility>

#include "draw.hpp"
#include "shape.hpp"

namespace drawing {

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
    virtual const shape::Shape& preview() const = 0;

    // 僅在 COMMIT 後呼叫一次；取出後由呼叫端銷毀工具。
    virtual std::unique_ptr<shape::Shape> takeShape() = 0;

    virtual ToolEventResult onMouseDown(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseUp(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMouseMove(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onMousePassiveMove(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onKeyDown(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onKeyUp(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onSpecialKeyDown(ToolEventState&) { return ToolEventResult::NONE; }
    virtual ToolEventResult onSpecialKeyUp(ToolEventState&) { return ToolEventResult::NONE; }
};

template <typename TShape>
class DrawingTool : public IDrawingTool {
   public:
    DrawingTool(GLfloat width, const GLfloat* color, const GLfloat* fillColor)
        : draft(std::make_unique<TShape>(width, color, fillColor)) {}

    const shape::Shape& preview() const override { return *draft; }
    std::unique_ptr<shape::Shape> takeShape() override { return std::move(draft); }

   protected:
    std::unique_ptr<TShape> draft;
};

std::unique_ptr<IDrawingTool> createDrawingTool(draw::Tool tool, GLfloat width, const GLfloat* color,
                                                const GLfloat* fillColor);

}  // namespace drawing
