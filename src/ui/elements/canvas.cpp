#include "ui/elements/canvas.hpp"

#include <utility>

#include "command/edit_command.hpp"

namespace paint::ui {

CanvasElement::CanvasElement(Document& document)
    : Element(), _gridVisible(true), _document(document), _currentTool(drawing::Tool::TOOL_PENCIL) {
    setFocusable(true);  // CanvasElement 可以接收鍵盤事件
    setHorizontalAlignment(Alignment::Stretch);
    setVerticalAlignment(Alignment::Stretch);

    _document.setCanvasSize(bounds().width, bounds().height);

    _currentStyle.stroke.width = 1;
    _currentStyle.stroke.color = ColorRGBA(Color::Black);
    _currentStyle.fill.color = ColorRGBA(Color::Transparent);
    _currentStyle.stroke.join = drawing::LineJoin::MITER;
    _currentStyle.stroke.cap = drawing::LineCap::ROUND;

    addEventListener<KeyDownEvent>([this](const KeyDownEvent& event) {
        // 忽略重複按鍵事件
        if (event.isRepeat()) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onKeyDown(event, localPosition));
    });

    addEventListener<KeyUpEvent>([this](const KeyUpEvent& event) {  // 忽略重複按鍵事件
        if (event.isRepeat()) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onKeyUp(event, localPosition));
    });

    addEventListener<ClickEvent>([this](const ClickEvent& event) {  // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onClick(event, localPosition));
    });

    addEventListener<DoubleClickEvent>(
        [this](const DoubleClickEvent& event) {  // 只處理左鍵點擊事件，其他按鍵忽略
            if (event.button() != MouseButton::MouseLeft) {
                return;
            }
            Point localPosition = windowToLocal(event.position());
            handleDraftEvent(_activeTool->onDoubleClick(event, localPosition));
        });

    addEventListener<MouseDownEvent>([this](const MouseDownEvent& event) {
        // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        captureMouse();  // 捕獲滑鼠事件，避免滑鼠移出畫布時無法接收 MouseUp 事件
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onMouseDown(event, localPosition));
    });

    addEventListener<MouseUpEvent>([this](const MouseUpEvent& event) {
        // 只處理左鍵點擊事件，其他按鍵忽略
        if (event.button() != MouseButton::MouseLeft) {
            return;
        }
        releaseMouseCapture();  // 釋放滑鼠事件捕獲
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onMouseUp(event, localPosition));
    });

    addEventListener<MouseMoveEvent>([this](const MouseMoveEvent& event) {
        // 如果沒有草稿，則不需要處理滑鼠移動事件，避免不必要的計算與渲染。
        if (!isDrawing()) {
            return;
        }
        Point localPosition = windowToLocal(event.position());
        handleDraftEvent(_activeTool->onMouseMove(event, localPosition));
    });

    resetTool();
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

    invalidateDisplay();
}

void CanvasElement::redo() {
    if (isDrawing()) {  // 畫畫時不可以 redo
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
    case drawing::ToolEventResult::UPDATE:  // 注意：這裡故意不 break，因為 COMMIT, CANCEL
                                            // 也需要重新繪製畫布
        invalidateDisplay();
        break;
    case drawing::ToolEventResult::NONE:
    default:
        // 不需要提交草稿，繼續繪製
        break;
    }
}

Size CanvasElement::measureContent(const Size& availableSize) {
    // 如果 availableSize 為 auto，則返回 (0, 0) 作為需求大小，表示畫布可以自由擴展。
    return {availableSize.width.value_or(0), availableSize.height.value_or(0)};
}

void CanvasElement::renderContent(RenderContext& context) {
    renderContent(context, _gridVisible);
}

void CanvasElement::renderContent(RenderContext& context, bool includeGrid) {
    auto oldVisibility = isGridVisible();
    setGridVisibility(includeGrid);

    _renderer.render(context, *this);

    setGridVisibility(oldVisibility);
}

}  // namespace paint::ui
