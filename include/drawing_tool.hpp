#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "canvas.hpp"
#include "point.hpp"
#include "shape.hpp"

// true -> 按下，false -> 釋放
typedef std::unordered_map<unsigned int, bool> KeyStateMap;  // 用於追蹤按鍵狀態的映射

namespace drawing {

struct ToolEventState {
    KeyStateMap& keyStates;         // 用於追蹤按鍵狀態的映射
    KeyStateMap& specialKeyStates;  // 用於追蹤特殊按鍵狀態的映射
    Point& mousePosition;           // 當前滑鼠位置
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
    DrawingTool(shape::ShapeStyle style) : draft(std::make_unique<TShape>(style)) {}

    const shape::Shape& preview() const override { return *draft; }
    std::unique_ptr<shape::Shape> takeShape() override { return std::move(draft); }

   protected:
    std::unique_ptr<TShape> draft;
};

std::unique_ptr<IDrawingTool> createDrawingTool(canvas::Tool tool, shape::ShapeStyle style);

}  // namespace drawing
