#include "element/canvas.hpp"

namespace paint {

CanvasElement::CanvasElement() : _currentTool(drawing::Tool::TOOL_PENCIL) {
    _currentStyle.stroke.width = 1;
    _currentStyle.stroke.color = ColorRGBA(Color::Black);
    _currentStyle.fill.color = ColorRGBA(Color::Transparent);
    _currentStyle.stroke.join = drawing::LineJoin::MITER;
    _currentStyle.stroke.cap = drawing::LineCap::ROUND;
}

void CanvasElement::undo() {
    if (isDrawing()) {  // 如果正在繪製草稿，則取消草稿
        clearDraft();
        invalidate();
    }

    if (_history.empty()) {  // 沒東西可以 undo
        return;
    }

    // 將最後一個歷史紀錄移到 redo stack
    _redoStack.push_back(std::move(_history.back()));
    _history.pop_back();
    invalidate();
}

void CanvasElement::redo() {
    if (isDrawing()) {  // 畫畫時不可以 redo
        return;
    }

    if (_redoStack.empty()) {  // 沒東西可以 redo
        return;
    }

    // 將最後一個 redo stack 移回歷史紀錄
    _history.push_back(std::move(_redoStack.back()));
    _redoStack.pop_back();
    invalidate();
}

void CanvasElement::clear() {
    _history.clear();
    _redoStack.clear();
    clearDraft();
    invalidate();
}

void CanvasElement::clearDraft() {
    _activeTool.reset();
}

void CanvasElement::createDraft() {
    if (isDrawing()) {
        clearDraft();
    }

    _activeTool = drawing::createDrawingTool(_currentTool, _currentStyle);
}

void CanvasElement::handleDraftEvent(drawing::ToolEventResult result) {
    switch (result) {
    case drawing::ToolEventResult::COMMIT:
        _history.push_back(_activeTool->takeShape());
        _redoStack.clear();  // 清除 redo stack，因為新的操作會使 redo stack 無效
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
    for (const auto& shape : _history) {
        _renderer.draw(*shape);
    }

    // 再繪製草稿
    if (isDrawing()) {
        _renderer.draw(*_activeTool->preview());
    }
}

void CanvasElement::onKeyDown(const KeyboardEvent& event) {
    // 處理 Ctrl+Z / Command+Z 以及 Ctrl+Shift+Z / Command+Shift+Z 的快捷鍵
    if (event.key() == Key::Z && event.keyboardState().isPrimaryModifierDown()) {
        if (event.keyboardState().isShiftDown()) {
            redo();
        } else {
            undo();
        }
        return;
    }

    // 忽略重複按鍵事件
    if (event.isRepeat()) {
        return;
    }

    if (isDrawing()) {
        handleDraftEvent(_activeTool->onKeyDown(event));
    }
}

void CanvasElement::onKeyUp(const KeyboardEvent& event) {
    // 忽略重複按鍵事件
    if (event.isRepeat()) {
        return;
    }

    if (isDrawing()) {
        handleDraftEvent(_activeTool->onKeyUp(event));
    }
}

void CanvasElement::onClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.button() != MouseButton::MouseLeft) {
        return;
    }

    if (isDrawing()) {
        handleDraftEvent(_activeTool->onClick(event));
    }
}

void CanvasElement::onDoubleClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.button() != MouseButton::MouseLeft) {
        return;
    }

    if (isDrawing()) {
        handleDraftEvent(_activeTool->onDoubleClick(event));
    }
}

void CanvasElement::onMouseDown(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.button() != MouseButton::MouseLeft) {
        return;
    }

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onMouseDown(event));
}

void CanvasElement::onMouseUp(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.button() != MouseButton::MouseLeft) {
        return;
    }

    if (isDrawing()) {
        handleDraftEvent(_activeTool->onMouseUp(event));
    }
}

void CanvasElement::onMouseMove(const MouseMoveEvent& event) {
    if (isDrawing()) {
        handleDraftEvent(_activeTool->onMouseMove(event));
    }
}

}  // namespace paint
