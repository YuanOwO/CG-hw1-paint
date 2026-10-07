#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <variant>
#include <vector>

#include "event/input_event.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"

namespace paint {

using KeyChordKey = std::variant<Key, Mod>;
using ShortcutAction = std::function<void()>;

class KeyChord {
   public:
    KeyChord(std::initializer_list<KeyChordKey> keys) : _keys(keys) {}

    bool matches(const KeyboardState& keyboard) const;
    bool contains(Key key) const;
    std::size_t size() const { return _keys.size(); }

   private:
    std::vector<KeyChordKey> _keys;
};

struct Shortcut {
    std::size_t id;
    KeyChord chord;
    ShortcutAction action;
};

class ShortcutManager {
   public:
    using ShortcutId = std::size_t;

    ShortcutId bind(KeyChord chord, ShortcutAction action);
    void unbind(ShortcutId id);

    bool handle(const KeyDownEvent& event);

   private:
    std::vector<Shortcut> _shortcuts;
    ShortcutId _nextId = 0;  // 用於生成唯一的快捷鍵 ID
};

}  // namespace paint
