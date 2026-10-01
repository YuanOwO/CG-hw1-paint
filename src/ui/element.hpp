#pragma once

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "render/render_context.hpp"
#include "ui/event_target.hpp"
#include "ui/layout/bounding.hpp"

namespace paint::ui {

class Window;

class Element : public EventTarget {
   public:
    Element() : _bounds(0, 0, 0, 0) {}
    Element(int width, int height) : _bounds(0, 0, width, height) { validateBounds(_bounds); }
    Element(int x, int y, int width, int height) : _bounds(x, y, width, height) { validateBounds(_bounds); }
    explicit Element(const BoundingBox& bounds) : _bounds(bounds) { validateBounds(_bounds); }

    virtual ~Element() = default;

    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = delete;
    Element& operator=(Element&&) = delete;

    // Geometry

    int x() const { return _bounds.x; }
    int y() const { return _bounds.y; }
    int width() const { return _bounds.width; }
    int height() const { return _bounds.height; }

    const BoundingBox& bounds() const { return _bounds; }

    bool contains(Point point) const { return _bounds.contains(point); }

    Element* hitTest(Point point) { return const_cast<Element*>(std::as_const(*this).hitTest(point)); }
    const Element* hitTest(Point point) const;

    // State

    bool isVisible() const { return _visible; }
    void setVisible(bool visible);

    bool isEnabled() const { return _enabled; }
    void setEnabled(bool enabled);

    bool isFocusable() const { return _focusable; }
    void setFocusable(bool focusable);

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

    void setBounds(const BoundingBox& bounds);

    // Interaction

    void captureMouse();
    void releaseMouseCapture();

    // Rendering

    void invalidate();
    virtual void renderContent(RenderContext& context) {}

    // Tree

    EventTarget* eventParent() const override;

   private:
    friend class Window;

    // Rendering

    void render(RenderContext& context);

    // Validation

    static void validateBounds(const BoundingBox& bounds) {
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Element size cannot be negative");
        }
    }

    // Geometry

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
