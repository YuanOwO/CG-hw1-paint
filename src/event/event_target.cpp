#include "event/event_target.hpp"

#include <type_traits>

namespace paint {

template <typename EventType, typename Callback>
std::size_t EventTarget::addEventListener(Callback&& callback) {
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

void EventTarget::removeEventListener(std::size_t id) {
    // 遍歷所有事件類型的監聽器列表，尋找並移除具有指定 id 的監聽器
    for (auto& [_, listeners] : _listeners) {
        auto it = std::remove_if(listeners.begin(), listeners.end(),
                                 [id](const ListenerEntry& entry) { return entry.id == id; });
        if (it != listeners.end()) {
            listeners.erase(it, listeners.end());
            break;  // 一旦找到並移除，立即退出循環
        }
    }
}

void EventTarget::dispatchEvent(Event& event) {
    event._target = this;
    event._propagationStopped = false;  // 重置事件的傳播停止狀態

    EventTarget* current = this;

    while (current != nullptr) {
        event._currentTarget = current;

        auto it = current->_listeners.find(typeid(event));

        if (it != current->_listeners.end()) {
            for (const auto& entry : it->second) {
                entry.callback(event);  // 調用 listener 的 callback 函數
            }
        }

        // 如果事件已停止傳播或不支持冒泡，則停止向上傳播
        if (event.propagationStopped() || !event.bubbles()) {
            break;
        }

        current = current->eventParent();  // 移動到父級事件目標
    }

    event._currentTarget = nullptr;  // 重置 currentTarget 為 nullptr，表示事件傳播結束
}

}  // namespace paint
