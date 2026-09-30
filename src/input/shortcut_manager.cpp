#include "input/shortcut_manager.hpp"

#include <algorithm>

namespace paint {

bool KeyChord::matches(const KeyboardState& keyboard) const {
    return std::all_of(_keys.begin(), _keys.end(), [&](const KeyChordKey& key) {
        // 使用 std::holds_alternative 和 std::get 來檢查 KeyChordKey 的類型，並根據類型調用相應的 isDown 方法
        if (std::holds_alternative<Key>(key)) {
            return keyboard.isDown(std::get<Key>(key));
        } else if (std::holds_alternative<Mod>(key)) {
            return keyboard.isDown(std::get<Mod>(key));
        }
        return false;
    });
}

bool KeyChord::contains(Key key) const {
    return std::any_of(_keys.begin(), _keys.end(), [&](const KeyChordKey& chordKey) {
        if (std::holds_alternative<Key>(chordKey)) {
            return std::get<Key>(chordKey) == key;
        } else if (std::holds_alternative<Mod>(chordKey)) {
            return std::get<Mod>(chordKey) == key;
        }
        return false;
    });
}

ShortcutManager::ShortcutId ShortcutManager::bind(KeyChord chord, ShortcutAction action) {
    const auto id = _nextId++;
    _shortcuts.push_back({id, std::move(chord), std::move(action)});
    return id;
}

void ShortcutManager::unbind(ShortcutId id) {
    auto it = std::remove_if(_shortcuts.begin(), _shortcuts.end(),
                             [id](const Shortcut& shortcut) { return shortcut.id == id; });

    _shortcuts.erase(it, _shortcuts.end());
}

bool ShortcutManager::handle(const KeyDownEvent& event) {
    if (event.isRepeat()) {
        return false;
    }

    const auto& keyboard = event.keyboardState();
    const Shortcut* matchedShortcut = nullptr;

    for (const auto& shortcut : _shortcuts) {
        if (!shortcut.chord.contains(event.key()) || !shortcut.chord.matches(keyboard)) {
            continue;
        }

        if (matchedShortcut == nullptr || shortcut.chord.size() > matchedShortcut->chord.size()) {
            matchedShortcut = &shortcut;
        }
    }

    if (matchedShortcut == nullptr) {
        return false;
    }

    matchedShortcut->action();
    return true;
}

}  // namespace paint
