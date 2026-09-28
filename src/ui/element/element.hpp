#pragma once

#include <functional>
#include <memory>
#include <utility>

#include "event/events.hpp"

namespace paint {

class Element {
   public:
    Element(int width, int height) : _width(width), _height(height) {}
    virtual ~Element() = default;

   protected:
    int _width, _height;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

    // 當元素需要重新渲染時，呼叫此函式通知父視窗
    void invalidate() {
        if (_invalidateCallback) {
            _invalidateCallback();
        }
    }

    virtual void render() {}

    virtual void onResize(const WindowResizeEvent& event) {
        _width = event.getWidth();
        _height = event.getHeight();
    }

    virtual void onKeyDown(const KeyboardEvent&) {}
    virtual void onKeyUp(const KeyboardEvent&) {}

    virtual void onMouseDown(const MouseEvent&) {}
    virtual void onMouseUp(const MouseEvent&) {}
    virtual void onMouseMove(const MouseMoveEvent&) {}

    virtual void onClick(const MouseClickEvent&) {}
    virtual void onDoubleClick(const MouseClickEvent&) {}

    virtual void onMouseEnter(const MouseEnterEvent&) {}
    virtual void onMouseLeave(const MouseLeaveEvent&) {}

   private:
    friend class Window;

    std::function<void()> _invalidateCallback;

    void setInvalidateCallback(std::function<void()> callback) { _invalidateCallback = std::move(callback); }
};

}  // namespace paint
