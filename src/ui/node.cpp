#include "ui/node.hpp"

#include "ui/window.hpp"

namespace paint::ui {

const Window* Node::window() const {
    const Node* current = this;

    // 往上走到根元素，然後返回其 window 指標
    while (current->_parent != nullptr) {
        current = current->_parent;
    }

    return current->_window;
}

Node& Node::appendChild(std::unique_ptr<Node> child) {
    if (!child) {
        throw std::invalid_argument("Child element cannot be null");
    }

    child->_parent = this;
    _children.push_back(std::move(child));

    onChildrenChanged();

    return *_children.back();
}

std::unique_ptr<Node> Node::removeChild(Node* child) {
    if (!child) {
        throw std::invalid_argument("Child element cannot be null");
    }

    auto it = std::find_if(_children.begin(), _children.end(),
                           [child](const std::unique_ptr<Node>& ptr) { return ptr.get() == child; });

    if (it == _children.end()) {
        throw std::invalid_argument("Child element not found");
    }

    // 必須在清除 parent 前通知 Window，才能辨識完整子樹並正常派送生命週期事件。
    if (auto* w = window()) {
        w->detachElementSubtree(child);
    }

    std::unique_ptr<Node> removedChild = std::move(*it);
    _children.erase(it);

    removedChild->_parent = nullptr;

    onChildrenChanged();

    return removedChild;
}

EventTarget* Node::eventParent() const {
    if (_parent) {
        return _parent;
    }
    return _window;
}

}  // namespace paint::ui
