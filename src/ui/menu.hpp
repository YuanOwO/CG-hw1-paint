#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "input/input_types.hpp"

namespace paint {

class Menu {
   public:
    using Action = std::function<void()>;

    Menu();
    ~Menu();

    // 禁止拷貝和賦值
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    int id() const { return _menuId; }

    bool isEnabled() const { return _attachedButton != MouseButton::Unknown; }

    void addMenuEntry(const std::string& label, Action action);
    Menu& addSubMenu(const std::string& label);

    void attach(MouseButton button);
    void detach();

   private:
    struct MenuItem {
        int id;
        std::string label;
        Action action;
    };

    int _windowId = 0;
    int _menuId = 0;
    int _nextItemId = 1;
    MouseButton _attachedButton = MouseButton::Unknown;

    std::vector<std::unique_ptr<Menu>> _submenus;
    std::unordered_map<int, MenuItem> _items;

    void _detach();

    static void menuCallback(int option);
};

}  // namespace paint
