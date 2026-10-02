#pragma once

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "render/render_context.hpp"
#include "ui/event_target.hpp"
#include "ui/layout/alignment.hpp"
#include "ui/layout/bounding.hpp"
#include "ui/layout/size.hpp"
#include "ui/layout/thickness.hpp"

namespace paint::ui {

class Window;

class Element : public EventTarget {
   public:
    Element() {}
    Element(Size size) : _preferredSize(size) {}
    Element(Size size, Margin margin, Padding padding)
        : _preferredSize(size), _margin(margin), _padding(padding) {}

    virtual ~Element() = default;

    // 禁止拷貝與移動操作，確保元素的唯一性
    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = delete;
    Element& operator=(Element&&) = delete;

    // Geometry

    int x() const { return _bounds.x; }
    int y() const { return _bounds.y; }
    int width() const { return _bounds.width; }
    int height() const { return _bounds.height; }

    const Size& preferredSize() const { return _preferredSize; }
    const Size& desiredSize() const { return _desiredSize; }
    const Margin& margin() const { return _margin; }
    const Padding& padding() const { return _padding; }
    const Alignment& horizontalAlignment() const { return _horizontalAlignment; }
    const Alignment& verticalAlignment() const { return _verticalAlignment; }
    const BoundingBox& bounds() const { return _bounds; }

    Element* hitTest(Point point) { return const_cast<Element*>(std::as_const(*this).hitTest(point)); }
    const Element* hitTest(Point point) const;

    // State

    bool isVisible() const { return _visible; }
    bool isEnabled() const { return _enabled; }
    bool isFocusable() const { return _focusable; }

    // Tree

    Window* window() { return const_cast<Window*>(std::as_const(*this).window()); }
    const Window* window() const;

    Element* parent() { return const_cast<Element*>(std::as_const(*this).parent()); }
    const Element* parent() const { return _parent; }

    const std::vector<std::unique_ptr<Element>>& children() const { return _children; }

    Element& appendChild(std::unique_ptr<Element> child);
    std::unique_ptr<Element> removeChild(Element* child);

   protected:
    // Geometry

    void setPreferredSize(const Size& size);
    void setMargin(const Margin& margin);
    void setPadding(const Padding& padding);
    void setHorizontalAlignment(Alignment alignment);
    void setVerticalAlignment(Alignment alignment);

    Size measure(const Size& availableSize);  // 傳入可用大小，返回元素的需求大小
    void arrange(const BoundingBox& bounds);  // 傳入元素的邊界，安排元素的佈局

    bool contains(Point point) const { return _bounds.contains(point); }

    virtual Size measureContent(const Size& availableSize) { return {0, 0}; }  // 由子類別實現
    virtual void arrangeContent(const BoundingBox& bounds) {}                  // 由子類別實現

    void invalidateLayout();

    // State

    void setVisible(bool visible);
    void setEnabled(bool enabled);
    void setFocusable(bool focusable);

    // Interaction

    void captureMouse();
    void releaseMouseCapture();

    // Rendering

    void invalidateDisplay();
    virtual void renderContent(RenderContext& context) {}

    // Tree

    EventTarget* eventParent() const override;

   private:
    friend class Window;

    // Rendering

    void render(RenderContext& context);

    // Geometry

    Size _preferredSize, _desiredSize;  // 優先大小與實際需求大小
    Margin _margin;
    Padding _padding;
    Alignment _horizontalAlignment = Alignment::Start;
    Alignment _verticalAlignment = Alignment::Start;
    BoundingBox _bounds;

    // State

    bool _visible = true;
    bool _enabled = true;
    bool _focusable = false;

    // Tree

    Window* _window = nullptr;
    Element* _parent = nullptr;
    std::vector<std::unique_ptr<Element>> _children;
};

}  // namespace paint::ui
