#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "event/event_target.hpp"
#include "ui/bounding.hpp"

namespace paint {

class Window;

class Element : public EventTarget {
   public:
    Element(int width, int height) : _bounds(0, 0, width, height) { validateBounds(_bounds); }
    Element(int x, int y, int width, int height) : _bounds(x, y, width, height) { validateBounds(_bounds); }
    explicit Element(const BoundingBox& bounds) : _bounds(bounds) { validateBounds(_bounds); }

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

    Window* window() const;
    Element* parent() const { return _parent; }
    const std::vector<std::unique_ptr<Element>>& children() const { return _children; }

    Element& appendChild(std::unique_ptr<Element> child);
    std::unique_ptr<Element> removeChild(Element* child);

    Element* hitTest(Point point);

   protected:
    EventTarget* eventParent() const override;

    void setBounds(const BoundingBox& bounds);

    // 當元素需要重新渲染時，呼叫此函式通知父視窗
    void invalidate();

    virtual void renderContent() {}

    virtual bool contains(Point point) const;

   private:
    friend class Window;  // 允許 Window 訪問 Element 的私有成員

    BoundingBox _bounds;
    Window* _window = nullptr;   // 只有根元素會有 window 指標，子元素的 window 指標為 nullptr
    Element* _parent = nullptr;  // 指向父元素的指標，若為 nullptr 則表示此元素為根元素
    std::vector<std::unique_ptr<Element>> _children;

    static void validateBounds(const BoundingBox& bounds) {
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }
    }
};

}  // namespace paint
