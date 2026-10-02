#pragma once

#include <memory>
#include <utility>

#include "common/point.hpp"
#include "drawing/shape.hpp"
#include "event/events.hpp"
#include "input/input_state.hpp"

namespace paint::drawing {

enum class Tool {
    TOOL_POINT,
    TOOL_PENCIL,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_ELLIPSE,
    TOOL_POLYGON,
};

enum class ToolEventResult {
    NONE,    // 表示草稿沒有任何變化，畫布不需要重新繪製草稿
    UPDATE,  // 表示草稿已更新，畫布需要重新繪製草稿
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

    virtual ToolEventResult finish() = 0;

    virtual ToolEventResult onKeyDown(const KeyboardEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onKeyUp(const KeyboardEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onClick(const ClickEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onDoubleClick(const ClickEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onMouseDown(const MouseButtonEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onMouseUp(const MouseButtonEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
    virtual ToolEventResult onMouseMove(const MouseMoveEvent& event, Point localPosition) {
        return ToolEventResult::NONE;
    }
};

template <typename TShape>
class DrawingTool : public IDrawingTool {
   public:
    DrawingTool(ShapeStyle style) : _style(style) {}

    const Shape* preview() const override { return _draft.get(); }
    std::unique_ptr<Shape> takeShape() override { return std::move(_draft); }

    ToolEventResult finish() override { return _draft ? ToolEventResult::COMMIT : ToolEventResult::NONE; }

   protected:
    void beginDraft() { _draft = std::make_unique<TShape>(_style); }

    ShapeStyle _style;                         // 繪圖工具的樣式資訊
    std::unique_ptr<TShape> _draft = nullptr;  // 草稿形狀，供畫布在 display 時繪製
};

std::unique_ptr<IDrawingTool> createDrawingTool(Tool tool, ShapeStyle style);

}  // namespace paint::drawing
