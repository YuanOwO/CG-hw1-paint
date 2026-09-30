#pragma once

#include "ui/event_target.hpp"

namespace paint::ui {

class Window;

class Node : public EventTarget {
   public:
    Node() = default;
    virtual ~Node() = default;

    // 禁止拷貝與移動操作，確保元素的唯一性
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;

    Window* window() { return const_cast<Window*>(std::as_const(*this).window()); }
    const Window* window() const;

    Node* parent() { return _parent; }
    const Node* parent() const { return _parent; }

    const std::vector<std::unique_ptr<Node>>& children() const { return _children; }

    Node& appendChild(std::unique_ptr<Node> child);
    std::unique_ptr<Node> removeChild(Node* child);

   protected:
    EventTarget* eventParent() const override;

    virtual void onChildrenChanged() {}

   private:
    friend class Window;  // 允許 Window 訪問 Element 的私有成員

    Window* _window = nullptr;  // 只有根元素會有 window 指標，子元素的 window 指標為 nullptr
    Node* _parent = nullptr;    // 指向父元素的指標，若為 nullptr 則表示此元素為根元素
    std::vector<std::unique_ptr<Node>> _children;
};

}  // namespace paint::ui
