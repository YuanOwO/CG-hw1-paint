#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include "event/event_target.hpp"
#include "event/events.hpp"
#include "ui/bounding.hpp"

namespace paint {

class Element : public EventTarget {
   public:
    Element(int width, int height) : _bounds(0, 0, width, height) {}

    Element(int x, int y, int width, int height) : _bounds(x, y, width, height) {
        if (width < 0 || height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }
    }

    Element(const BoundingBox& bounds) : _bounds(bounds) {
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }
    }

    virtual ~Element() = default;

    // 禁止拷貝與移動操作，確保元素的唯一性
    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = delete;
    Element& operator=(Element&&) = delete;

    int x() const { return _bounds.x; }
    int y() const { return _bounds.y; }
    int width() const { return _bounds.width; }
    int height() const { return _bounds.height; }

    BoundingBox bounds() const { return _bounds; }

    Element& appendChild(std::unique_ptr<Element> child);
    std::unique_ptr<Element> removeChild(Element* child);

    Element* parent() const { return _parent; }
    const std::vector<std::unique_ptr<Element>>& children() const { return _children; }

   protected:
    void setBounds(const BoundingBox& bounds) {
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }

        const auto oldBounds = _bounds;
        if (oldBounds.x == bounds.x && oldBounds.y == bounds.y && oldBounds.width == bounds.width &&
            oldBounds.height == bounds.height) {
            return;  // No change in bounds, no need to invalidate
        }

        _bounds = bounds;
        onResize(WindowResizeEvent(bounds.width, bounds.height));
        invalidate();
    }

    // 當元素需要重新渲染時，呼叫此函式通知父視窗
    void invalidate() {
        if (_parent) {
            _parent->invalidate();
        } else if (_invalidateCallback) {
            _invalidateCallback();
        }
    }

    virtual void renderContent() {}

    virtual void onResize(const WindowResizeEvent& event) {
        _bounds.width = event.width();
        _bounds.height = event.height();
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
    BoundingBox _bounds;
    Element* _parent = nullptr;  // 指向父元素的指標，若為 nullptr 則表示此元素為根元素
    std::vector<std::unique_ptr<Element>> _children;
    std::function<void()> _invalidateCallback;

    friend class Window;

    void setInvalidateCallback(std::function<void()> callback) { _invalidateCallback = std::move(callback); }
};

}  // namespace paint
