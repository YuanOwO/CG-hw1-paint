#pragma once

#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "common/point.hpp"
#include "drawing/scene_object.hpp"
#include "event/events.hpp"

namespace paint::drawing {

// 工具可在場景之上提供暫時性的視覺內容。之後選取工具可在這裡加入
// selection bounds、marquee、handles 等 overlay 資訊。
struct ToolOverlay {
    std::vector<const SceneObject*> previewObjects;
};

struct NoAction {};

struct AddObjectAction {
    std::unique_ptr<SceneObject> object;
};

using ToolAction = std::variant<NoAction, AddObjectAction>;

// 工具事件的結果。action 描述要交給編輯器執行的動作，工具本身不直接修改 Document。
struct ToolResult {
    bool handled = false;
    bool needsRedraw = false;
    ToolAction action;

    static ToolResult redraw(bool handled = false) { return {handled, true, NoAction{}}; }

    static ToolResult addObject(std::unique_ptr<SceneObject> object, bool handled = false) {
        return {handled, true, AddObjectAction{std::move(object)}};
    }
};

// 所有畫布工具的共同介面。工具不一定會建立物件，例如未來的選取工具。
class ICanvasTool {
   public:
    virtual ~ICanvasTool() = default;

    virtual ToolOverlay overlay() const { return {}; }
    virtual bool isInteracting() const { return false; }

    virtual ToolResult deactivate() { return {}; }  // 用於切換工具
    virtual ToolResult cancel() { return {}; }      // 用於 Escape、Undo 等取消操作

    virtual ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onKeyUp(const KeyboardEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onTextInput(const TextInputEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onClick(const ClickEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onDoubleClick(const ClickEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition) { return {}; }
    virtual ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) { return {}; }
};

}  // namespace paint::drawing
