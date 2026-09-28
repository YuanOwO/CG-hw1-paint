#include "ui/element/canvas_element.hpp"

#include <utility>

#include "command/edit_command.hpp"

namespace paint {

CanvasElement::CanvasElement(Document& document)
    : _document(document), _currentTool(drawing::Tool::TOOL_PENCIL) {
    _currentStyle.stroke.width = 1;
    _currentStyle.stroke.color = ColorRGBA(Color::Black);
    _currentStyle.fill.color = ColorRGBA(Color::Transparent);
    _currentStyle.stroke.join = drawing::LineJoin::MITER;
    _currentStyle.stroke.cap = drawing::LineCap::ROUND;
}

void CanvasElement::undo() {
    if (isDrawing()) {  // 如果正在繪製草稿，則取消草稿
        clearDraft();
    } else {
        _document.undo();
    }

    invalidate();
}

void CanvasElement::redo() {
    if (isDrawing()) {  // 畫畫時不可以 redo
        return;
    }

    _document.redo();
    invalidate();
}

void CanvasElement::clear() {
    _document.clearScene();
    clearDraft();
    invalidate();
}

void CanvasElement::clearDraft() {
    _activeTool.reset();
}

void CanvasElement::createDraft() {
    if (isDrawing()) {
        return;  // 已經有草稿了，不需要再創建
    }

    _activeTool = drawing::createDrawingTool(_currentTool, _currentStyle);
}

void CanvasElement::handleDraftEvent(drawing::ToolEventResult result) {
    switch (result) {
    case drawing::ToolEventResult::COMMIT:
        _document.addShape(_activeTool->takeShape());
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
    for (const auto& shape : _document.getScene().getShapes()) {
        _renderer.draw(*shape);
    }

    // 再繪製草稿
    if (isDrawing()) {
        _renderer.draw(*_activeTool->preview());
    }
}

void CanvasElement::onKeyDown(const KeyboardEvent& event) {
    // 處理 Ctrl+Z / Command+Z 以及 Ctrl+Shift+Z / Command+Shift+Z 的快捷鍵
    if (event.getKey() == Key::Z && event.getKeyboardState().isPrimaryModifierDown()) {
        if (event.getKeyboardState().isShiftDown()) {
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

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onKeyDown(event));
}

void CanvasElement::onKeyUp(const KeyboardEvent& event) {
    // 忽略重複按鍵事件
    if (event.isRepeat()) {
        return;
    }

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onKeyUp(event));
}

void CanvasElement::onClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }
    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onClick(event));
}

void CanvasElement::onDoubleClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onDoubleClick(event));
}

void CanvasElement::onMouseDown(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onMouseDown(event));
}

void CanvasElement::onMouseUp(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    if (!isDrawing()) {
        createDraft();
    }

    handleDraftEvent(_activeTool->onMouseUp(event));
}

void CanvasElement::onMouseMove(const MouseMoveEvent& event) {
    // 如果沒有草稿，則不需要處理滑鼠移動事件，避免不必要的計算與渲染。
    if (!isDrawing()) {
        return;
    }

    handleDraftEvent(_activeTool->onMouseMove(event));
}

}  // namespace paint
