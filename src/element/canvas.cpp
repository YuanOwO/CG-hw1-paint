#include "element/canvas.hpp"

namespace paint {

CanvasElement::CanvasElement() : currentTool(drawing::Tool::TOOL_PENCIL) {
    currentStyle.stroke.width = 1;
    currentStyle.stroke.color = ColorRGBA(Color::Black);
    currentStyle.fill.color = ColorRGBA(Color::Transparent);
    currentStyle.stroke.join = drawing::LineJoin::MITER;
    currentStyle.stroke.cap = drawing::LineCap::ROUND;
}

void CanvasElement::undo() {
    if (isDrawing()) {  // 如果正在繪製草稿，則取消草稿
        clearDraft();
        invalidate();
    }

    if (history.empty()) {  // 沒東西可以 undo
        return;
    }

    // 將最後一個歷史紀錄移到 redo stack
    redoStack.push_back(std::move(history.back()));
    history.pop_back();
    invalidate();
}

void CanvasElement::redo() {
    if (isDrawing()) {  // 畫畫時不可以 redo
        return;
    }

    if (redoStack.empty()) {  // 沒東西可以 redo
        return;
    }

    // 將最後一個 redo stack 移回歷史紀錄
    history.push_back(std::move(redoStack.back()));
    redoStack.pop_back();
    invalidate();
}

void CanvasElement::clear() {
    history.clear();
    redoStack.clear();
    clearDraft();
    invalidate();
}

void CanvasElement::clearDraft() {
    activeTool.reset();
}

void CanvasElement::createDraft() {
    if (isDrawing()) {
        clearDraft();
    }

    activeTool = drawing::createDrawingTool(currentTool, currentStyle);
}

void CanvasElement::handleDraftEvent(drawing::ToolEventResult result) {
    switch (result) {
    case drawing::ToolEventResult::COMMIT:
        history.push_back(activeTool->takeShape());
        redoStack.clear();  // 清除 redo stack，因為新的操作會使 redo stack 無效
        [[fallthrough]];
    case drawing::ToolEventResult::CANCEL:  // 注意：這裡故意不 break，因為 COMMIT 也需要清除草稿
        clearDraft();
        break;
    case drawing::ToolEventResult::NONE:
    default:
        // 不需要提交草稿，繼續繪製
        break;
    }

    invalidate();
}

void CanvasElement::render() {
    // 先繪製歷史紀錄
    for (const auto& shape : history) {
        shape->draw();
    }

    // 再繪製草稿
    if (isDrawing()) {
        activeTool->preview().draw();
    }
}

void CanvasElement::onKeyDown(const KeyboardEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onKeyDown(state));
    }
}

void CanvasElement::onKeyUp(const KeyboardEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onKeyUp(state));
    }
}

void CanvasElement::onClick(const MouseClickEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onClick(state));
    }
}

void CanvasElement::onDoubleClick(const MouseClickEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onDoubleClick(state));
    }
}

void CanvasElement::onMouseDown(const MouseEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(activeTool->onMouseDown(state));
}

void CanvasElement::onMouseUp(const MouseEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onMouseUp(state));
    }
}

void CanvasElement::onMouseMove(const MouseMoveEvent& event) {
    drawing::ToolEventState state(event.keyboardState(), event.mouseState());

    if (isDrawing()) {
        handleDraftEvent(activeTool->onMouseMove(state));
    }
}

}  // namespace paint
