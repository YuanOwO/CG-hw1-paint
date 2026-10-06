#include "ui/elements/canvas.hpp"

#include <utility>

#include "command/edit_command.hpp"
#include "drawing/text_object.hpp"

namespace paint::ui {

CanvasElement::CanvasElement(Document& document)
    : Element(), _document(document), _currentTool(drawing::ToolKind::PENCIL) {
    setFocusable(true);  // CanvasElement 可以接收鍵盤事件
    setHorizontalAlignment(Alignment::Stretch);
    setVerticalAlignment(Alignment::Stretch);

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        // 忽略重複按鍵事件
        if (event.isRepeat()) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        if (handleToolResult(_activeTool->onKeyDown(event, localPosition))) {
            event.stopPropagation();
        }
    });

    addEventListener<KeyUpEvent>([this](KeyUpEvent& event) {  // 忽略重複按鍵事件
        if (event.isRepeat()) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        if (handleToolResult(_activeTool->onKeyUp(event, localPosition))) {
            event.stopPropagation();
        }
    });

    addEventListener<TextInputEvent>([this](TextInputEvent& event) {
        Point localPosition = windowToLocal(event.position());
        if (handleToolResult(_activeTool->onTextInput(event, localPosition))) {
            event.stopPropagation();
        }
    });

    addEventListener<ClickEvent>([this](const ClickEvent& event) {  // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        handleToolResult(_activeTool->onClick(event, localPosition));
    });

    addEventListener<DoubleClickEvent>(
        [this](const DoubleClickEvent& event) {  // 只處理左鍵點擊事件，其他按鍵忽略
            if (event.button() != MouseButton::MouseLeft) {
                return;
            }
            Point localPosition = windowToLocal(event.position());
            handleToolResult(_activeTool->onDoubleClick(event, localPosition));
        });

    addEventListener<MouseDownEvent>([this](const MouseDownEvent& event) {
        // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        captureMouse();  // 捕獲滑鼠事件，避免滑鼠移出畫布時無法接收 MouseUp 事件
        Point localPosition = windowToLocal(event.position());
        handleToolResult(_activeTool->onMouseDown(event, localPosition));
    });

    addEventListener<MouseUpEvent>([this](const MouseUpEvent& event) {
        // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        releaseMouseCapture();  // 釋放滑鼠事件捕獲
        Point localPosition = windowToLocal(event.position());
        handleToolResult(_activeTool->onMouseUp(event, localPosition));
    });

    addEventListener<MouseMoveEvent>([this](const MouseMoveEvent& event) {
        Point localPosition = windowToLocal(event.position());
        handleToolResult(_activeTool->onMouseMove(event, localPosition));
    });

    resetTool();
}

void CanvasElement::setTool(drawing::ToolKind tool) {
    if (_currentTool == tool) {
        return;  // 工具沒有改變，不需要重置
    }

    // 由目前工具自行決定切換工具時要提交或取消操作。
    handleToolResult(_activeTool->deactivate());

    _currentTool = tool;
    resetTool();
}

void CanvasElement::setCurrentStyle(const drawing::StyleSet& style) {
    _currentStyle = style;

    if (!isInteracting()) {
        resetTool();
    }

    if (_styleChangedHandler) {
        _styleChangedHandler(_currentStyle);
    }
}

void CanvasElement::undo() {
    if (isInteracting()) {  // 操作中先取消工具狀態，不影響文件歷史
        handleToolResult(_activeTool->cancel());
        releaseMouseCapture();
    } else {
        _document.undo();
    }

    invalidateDisplay();
}

void CanvasElement::redo() {
    if (isInteracting()) {  // 操作時不可以 redo
        return;
    }

    _document.redo();
    invalidateDisplay();
}

void CanvasElement::clear() {
    _document.clearScene();
    resetTool();
    invalidateDisplay();
}

void CanvasElement::insertText(Point anchor, std::string text, drawing::PaintStyle paint,
                               drawing::TextStyle style) {
    if (text.empty()) {
        return;
    }

    _document.addObject(
        std::make_unique<drawing::TextObject>(anchor, std::move(text), paint, std::move(style)));
    invalidateDisplay();
}

void CanvasElement::resetTool() {
    _activeTool = drawing::createCanvasTool(_currentTool, _document.scene(), _currentStyle);
}

bool CanvasElement::handleToolResult(drawing::ToolResult result) {
    const bool handled = result.handled;

    if (auto* addObject = std::get_if<drawing::AddObjectAction>(&result.action)) {
        if (addObject->object) {
            _document.addObject(std::move(addObject->object));
        }
    }

    if (auto* removeObject = std::get_if<drawing::RemoveObjectAction>(&result.action)) {
        if (removeObject->object) {
            _document.removeObject(std::move(removeObject->object));
        }
    }

    if (auto* request = std::get_if<drawing::RequestTextInputAction>(&result.action)) {
        if (_textInputRequestHandler) {
            _textInputRequestHandler(request->anchor, request->paint, request->style);
        }
    }

    if (result.needsRedraw) {
        invalidateDisplay();
    }

    return handled;
}

Size CanvasElement::measureContent(const Size& availableSize) {
    // 如果 availableSize 為 auto，則返回 (0, 0) 作為需求大小，表示畫布可以自由擴展。
    return {availableSize.width.value_or(0), availableSize.height.value_or(0)};
}

void CanvasElement::renderContent(render::RenderContext& context) {
    _renderer.render(context, *this);
}

}  // namespace paint::ui
