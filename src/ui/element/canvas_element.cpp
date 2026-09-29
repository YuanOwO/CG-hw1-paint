#include "ui/element/canvas_element.hpp"

#include <utility>

#include "command/edit_command.hpp"

namespace paint {

CanvasElement::CanvasElement(int width, int height, Document& document)
    : Element(width, height), _showGrid(true), _document(document), _currentTool(drawing::Tool::TOOL_PENCIL) {
    _document.setCanvasSize(width, height);
    _currentStyle.stroke.width = 1;
    _currentStyle.stroke.color = ColorRGBA(Color::Black);
    _currentStyle.fill.color = ColorRGBA(Color::Transparent);
    _currentStyle.stroke.join = drawing::LineJoin::MITER;
    _currentStyle.stroke.cap = drawing::LineCap::ROUND;

    resetTool();
}

void CanvasElement::onResize(const WindowResizeEvent& event) {
    _document.setCanvasSize(event.getWidth(), event.getHeight());
    Element::onResize(event);
}

void CanvasElement::setTool(drawing::Tool tool) {
    if (_currentTool == tool) {
        return;  // 工具沒有改變，不需要重置
    }

    // 如果目前正在繪製草稿，則先提交，再切換工具
    handleDraftEvent(_activeTool->finish());

    _currentTool = tool;
    resetTool();
}

void CanvasElement::setStyle(const drawing::ShapeStyle& style) {
    _currentStyle = style;

    if (!isDrawing()) {
        resetTool();
    }
}

void CanvasElement::undo() {
    if (isDrawing()) {  // 如果正在繪製草稿，則取消草稿
        resetTool();
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
    resetTool();
    invalidate();
}

void CanvasElement::newFile() {
    _document.newFile();
    resetTool();
    invalidate();
}

void CanvasElement::resetTool() {
    _activeTool = drawing::createDrawingTool(_currentTool, _currentStyle);
}

void CanvasElement::handleDraftEvent(drawing::ToolEventResult result) {
    switch (result) {
    case drawing::ToolEventResult::COMMIT:
        _document.addShape(_activeTool->takeShape());
        [[fallthrough]];
    case drawing::ToolEventResult::CANCEL:  // 注意：這裡故意不 break，因為 COMMIT 也需要清除草稿
        resetTool();
        [[fallthrough]];
    case drawing::ToolEventResult::UPDATE:  // 注意：這裡故意不 break，因為 COMMIT, CANCEL 也需要重新繪製畫布
        invalidate();
        break;
    case drawing::ToolEventResult::NONE:
    default:
        // 不需要提交草稿，繼續繪製
        break;
    }
}

void CanvasElement::renderContent() {
    renderContent(_showGrid);
}

void CanvasElement::renderContent(bool includeGrid) {
    if (includeGrid) {
        _renderer.drawGrid(_width, _height);
    }

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

    handleDraftEvent(_activeTool->onKeyDown(event));
}

void CanvasElement::onKeyUp(const KeyboardEvent& event) {
    // 忽略重複按鍵事件
    if (event.isRepeat()) {
        return;
    }

    handleDraftEvent(_activeTool->onKeyUp(event));
}

void CanvasElement::onClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    handleDraftEvent(_activeTool->onClick(event));
}

void CanvasElement::onDoubleClick(const MouseClickEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    handleDraftEvent(_activeTool->onDoubleClick(event));
}

void CanvasElement::onMouseDown(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
    }

    handleDraftEvent(_activeTool->onMouseDown(event));
}

void CanvasElement::onMouseUp(const MouseEvent& event) {
    // 只處理左鍵點擊事件，其他按鍵忽略
    if (event.getButton() != MouseButton::MouseLeft) {
        return;
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
