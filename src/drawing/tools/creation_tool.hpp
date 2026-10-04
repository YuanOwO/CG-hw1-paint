#pragma once

#include <memory>
#include <utility>

#include "drawing/shape_object.hpp"
#include "drawing/tools/canvas_tool.hpp"

namespace paint::drawing {

// 建立新場景物件的工具共用基底。它不假設物件的樣式或建構方式，
// 因此 ShapeObject、TextObject 等不同種類都能使用。
template <typename TObject>
class CreationTool : public ICanvasTool {
   public:
    ToolOverlay overlay() const override {
        ToolOverlay result;
        if (_draft) {
            result.previewObjects.push_back(_draft.get());
        }
        return result;
    }

    bool isInteracting() const override { return _draft != nullptr; }

    ToolResult deactivate() override { return commitDraft(); }
    ToolResult cancel() override { return cancelDraft(); }

   protected:
    template <typename... Args>
    void beginDraft(Args&&... args) {
        _draft = std::make_unique<TObject>(std::forward<Args>(args)...);
    }

    ToolResult commitDraft(bool handled = false) {
        if (!_draft) {
            return {};
        }

        std::unique_ptr<SceneObject> object = std::move(_draft);
        return ToolResult::addObject(std::move(object), handled);
    }

    ToolResult cancelDraft(bool handled = false) {
        if (!_draft) {
            return {};
        }

        _draft.reset();
        return ToolResult::redraw(handled);
    }

    std::unique_ptr<TObject> _draft;
};

// 現有幾何繪圖工具共用 ShapeStyle；文字工具可直接繼承 CreationTool<TextObject>，
// 並用自己的 TextStyle 建立草稿。
template <typename TShape>
class ShapeCreationTool : public CreationTool<TShape> {
   public:
    explicit ShapeCreationTool(ShapeStyle style) : _style(std::move(style)) {}

   protected:
    void beginShapeDraft() { this->beginDraft(_style); }

    ShapeStyle _style;
};

}  // namespace paint::drawing
