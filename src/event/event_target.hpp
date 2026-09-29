#pragma once

#include <cstddef>
#include <functional>
#include <typeindex>
#include <unordered_map>

#include "event/event.hpp"

namespace paint {

class EventTarget {
   public:
    virtual ~EventTarget() = default;

    template <typename EventType, typename Callback>
    std::size_t addEventListener(Callback&& callback);

    void removeEventListener(std::size_t id);

    // 分派事件給當前目標，並沿著事件目標樹向上冒泡（如果事件支持冒泡）
    void dispatchEvent(Event& event);

   protected:
    virtual EventTarget* eventParent() const { return nullptr; }  // 默認沒有父級事件目標

   private:
    using Listener = std::function<void(Event&)>;

    struct ListenerEntry {
        std::size_t id;
        Listener callback;
    };

    std::unordered_map<std::type_index, std::vector<ListenerEntry>> _listeners;
    std::size_t _nextListenerId = 0;
};

}  // namespace paint
