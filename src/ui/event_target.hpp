#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include "event/event.hpp"

namespace paint::ui {

class EventTarget {
   public:
    using ListenerId = std::size_t;
    using Listener = std::function<void(Event&)>;

    EventTarget() = default;
    virtual ~EventTarget() = default;

    // 禁止拷貝與移動操作，確保事件目標的唯一性
    EventTarget(const EventTarget&) = delete;
    EventTarget& operator=(const EventTarget&) = delete;
    EventTarget(EventTarget&&) = delete;
    EventTarget& operator=(EventTarget&&) = delete;

    template <typename EventType, typename Callback>
    ListenerId addEventListener(Callback&& callback) {
        // 靜態斷言，確保 EventType 是 Event 的衍生類
        static_assert(std::is_base_of_v<Event, EventType>, "EventType must be derived from Event");

        const auto id = _nextListenerId++;

        // 將 callback 包裝成 Listener，確保其符合 Listener 的簽名
        auto listener = [callback = std::forward<Callback>(callback)](Event& event) {
            callback(static_cast<EventType&>(event));  // 將 Event& 轉換為 EventType&，並調用 callback
        };

        _listeners[typeid(EventType)].push_back({id, listener});

        return id;
    }

    void removeEventListener(ListenerId id);

    // 分派事件給當前目標，並沿著事件目標樹向上冒泡（如果事件支持冒泡）
    void dispatchEvent(Event& event);

   protected:
    virtual EventTarget* eventParent() const { return nullptr; }  // 默認沒有父級事件目標

   private:
    struct ListenerEntry {
        ListenerId id;
        Listener callback;
    };

    std::unordered_map<std::type_index, std::vector<ListenerEntry>> _listeners;
    ListenerId _nextListenerId = 0;
};

}  // namespace paint::ui
