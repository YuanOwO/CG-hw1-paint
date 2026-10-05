#pragma once

#include <memory>

#include "drawing/scene.hpp"
#include "drawing/tools/canvas_tool.hpp"

namespace paint::drawing {

// 選取工具只維護暫時的選取狀態；刪除等文件修改仍交由 CanvasElement 執行。
class SelectTool : public ICanvasTool {
   public:
    explicit SelectTool(const Scene& scene) : _scene(scene) {}

    ToolOverlay overlay() const override;

    ToolResult cancel() override;
    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override;
    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override;

   private:
    const Scene& _scene;
    std::shared_ptr<SceneObject> _selected;
};

}  // namespace paint::drawing
