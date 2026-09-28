#pragma once

#include <functional>
#include <memory>

#include "event/event.hpp"

namespace paint {

class Element {
   public:
    virtual ~Element() = default;

   protected:
    Element() = default;

    // 當元素需要重新渲染時，呼叫此函式通知父視窗
    void invalidate() {
        if (_invalidateCallback) {
            _invalidateCallback();
        }
    }

    virtual void render() {}

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

using ElementPtr = std::unique_ptr<Element>;

}  // namespace paint
