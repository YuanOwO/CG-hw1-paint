#pragma once

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "render/render_context.hpp"
#include "ui/layout/bounding.hpp"
#include "ui/node.hpp"

namespace paint::ui {

class Window;

class Element : public Node {
   public:
    Element() : _bounds(0, 0, 0, 0) {}
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

    const BoundingBox& bounds() const { return _bounds; }

    bool isVisible() const { return _visible; }
    void setVisible(bool visible);

    bool isEnabled() const { return _enabled; }
    void setEnabled(bool enabled);

    bool isFocusable() const { return _focusable; }
    void setFocusable(bool focusable);

    Element* hitTest(Point point) { return const_cast<Element*>(std::as_const(*this).hitTest(point)); }
    const Element* hitTest(Point point) const;

   protected:
    void setBounds(const BoundingBox& bounds);

    // 當元素需要重新渲染時，呼叫此函式通知父視窗
    void invalidate();

    // 當元素需要捕獲滑鼠事件時，呼叫此函式通知父視窗
    void captureMouse();
    void releaseMouseCapture();

    virtual void renderContent(RenderContext& context) {}

    virtual bool contains(Point point) const { return _bounds.contains(point); }

    void onChildrenChanged() override { invalidate(); }

   private:
    friend class Window;  // 允許 Window 訪問 Element 的私有成員

    BoundingBox _bounds;

    bool _visible = true;     // 元素是否可見，默認為可見
    bool _enabled = true;     // 元素是否可用，默認為可用
    bool _focusable = false;  // 元素是否可聚焦，默認為不可聚焦

    void render(RenderContext& context);

    static void validateBounds(const BoundingBox& bounds) {
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }
    }
};

}  // namespace paint::ui
