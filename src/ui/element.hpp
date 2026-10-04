#pragma once

#include <chrono>
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
    void setPreferredSize(const Size& size);

    const Size& desiredSize() const { return _desiredSize; }

    const Margin& margin() const { return _margin; }
    void setMargin(const Margin& margin);

    const Padding& padding() const { return _padding; }
    void setPadding(const Padding& padding);

    const Alignment& horizontalAlignment() const { return _horizontalAlignment; }
    void setHorizontalAlignment(Alignment alignment);

    const Alignment& verticalAlignment() const { return _verticalAlignment; }
    void setVerticalAlignment(Alignment alignment);

    const BoundingBox& bounds() const { return _bounds; }

    Point windowToLocal(Point point) const;
    Point localToWindow(Point point) const;

    Size measure(const Size& availableSize);  // 傳入可用大小，返回元素的需求大小
    void arrange(const BoundingBox& slots);   // 傳入元素的邊界，安排元素的佈局

    Element* hitTest(Point point) { return const_cast<Element*>(std::as_const(*this).hitTest(point)); }
    const Element* hitTest(Point point) const;

    // State

    bool isVisible() const { return _visible; }
    void setVisible(bool visible);

    bool isEnabled() const { return _enabled; }
    void setEnabled(bool enabled);

    bool isFocusable() const { return _focusable; }
    void setFocusable(bool focusable);

    const ColorRGBA& backgroundColor() const { return _backgroundColor; }
    void setBackgroundColor(const ColorRGBA& color);

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

    bool contains(Point point) const { return _bounds.contains(point); }

    virtual Size measureContent(const Size& availableSize) { return {0, 0}; }  // 由子類別實現
    virtual void arrangeContent(const BoundingBox& contentBounds) {}           // 由子類別實現

    void invalidateLayout();

    // Interaction

    void captureMouse();
    void releaseMouseCapture();

    // Rendering

    void invalidateDisplay();
    virtual void renderContent(render::RenderContext& context) {}
    virtual void renderOverlay(render::RenderContext& context) {}
    virtual void update(std::chrono::milliseconds delta) {}

    // Tree

    EventTarget* eventParent() const override;

   private:
    friend class Window;

    // Geometry

    Size _preferredSize;                     // 元素的首選大小，可能為 auto
    Size _desiredSize, _desiredElementSize;  // 實際需求大小(含 margin)、內容需求大小(不含 margin)
    Margin _margin;
    Padding _padding;
    Alignment _horizontalAlignment = Alignment::Start;
    Alignment _verticalAlignment = Alignment::Start;
    BoundingBox _bounds;

    // State

    bool _visible = true;
    bool _enabled = true;
    bool _focusable = false;

    // Rendering

    ColorRGBA _backgroundColor = Color::Transparent;
    void render(render::RenderContext& context);
    void updateTree(std::chrono::milliseconds delta);

    // Tree

    Window* _window = nullptr;
    Element* _parent = nullptr;
    std::vector<std::unique_ptr<Element>> _children;
};

}  // namespace paint::ui
