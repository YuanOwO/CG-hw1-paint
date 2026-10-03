#include "ui/menu.hpp"

#include <GL/freeglut.h>

#include <utility>

#include "ui/window.hpp"

namespace paint::ui {

namespace {

std::unordered_map<int, Menu*> menus;

}  // namespace

Menu::Menu() {
    _windowId = glutGetWindow();
    _menuId = glutCreateMenu(menuCallback);
    menus[_menuId] = this;
}

Menu::~Menu() {
    if (_menuId == 0) {
        return;
    }

    menus.erase(_menuId);

    if (glutGet(GLUT_INIT_STATE)) {
        glutDestroyMenu(_menuId);
    }

    _menuId = 0;
}

void Menu::addMenuEntry(const std::string& label, MenuAction action) {
    const auto prevWindowId = glutGetWindow();
    const auto prevMenuId = glutGetMenu();

    glutSetWindow(_windowId);
    glutSetMenu(_menuId);

    int itemId = _nextItemId++;
    _items.emplace(itemId, MenuItem{itemId, label, std::move(action)});
    glutAddMenuEntry(label.c_str(), itemId);

    if (prevWindowId != 0) {
        glutSetWindow(prevWindowId);
    }
    if (prevMenuId != 0) {
        glutSetMenu(prevMenuId);
    }
}

Menu& Menu::addSubMenu(const std::string& label) {
    const auto prevWindowId = glutGetWindow();
    const auto prevMenuId = glutGetMenu();

    glutSetWindow(_windowId);

    auto submenu = std::make_unique<Menu>();
    Menu& submenuRef = *submenu;

    glutSetMenu(_menuId);

    glutAddSubMenu(label.c_str(), submenuRef._menuId);
    _submenus.push_back(std::move(submenu));

    if (prevWindowId != 0) {
        glutSetWindow(prevWindowId);
    }
    if (prevMenuId != 0) {
        glutSetMenu(prevMenuId);
    }

    return submenuRef;
}

void Menu::attach(MouseButton button) {
    const auto prevWindowId = glutGetWindow();
    const auto prevMenuId = glutGetMenu();

    glutSetWindow(_windowId);
    glutSetMenu(_menuId);

    _detach();  // 先解除之前的綁定

    _attachedButton = button;

    if (button == MouseButton::MouseLeft) {
        glutAttachMenu(GLUT_LEFT_BUTTON);
    } else if (button == MouseButton::MouseRight) {
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    } else if (button == MouseButton::MouseMiddle) {
        glutAttachMenu(GLUT_MIDDLE_BUTTON);
    }

    if (prevWindowId != 0) {
        glutSetWindow(prevWindowId);
    }
    if (prevMenuId != 0) {
        glutSetMenu(prevMenuId);
    }
}

void Menu::detach() {
    const auto prevWindowId = glutGetWindow();
    const auto prevMenuId = glutGetMenu();

    glutSetWindow(_windowId);
    glutSetMenu(_menuId);

    _detach();

    if (prevWindowId != 0) {
        glutSetWindow(prevWindowId);
    }
    if (prevMenuId != 0) {
        glutSetMenu(prevMenuId);
    }
}

void Menu::_detach() {
    if (_attachedButton == MouseButton::MouseLeft) {
        glutDetachMenu(GLUT_LEFT_BUTTON);
    } else if (_attachedButton == MouseButton::MouseRight) {
        glutDetachMenu(GLUT_RIGHT_BUTTON);
    } else if (_attachedButton == MouseButton::MouseMiddle) {
        glutDetachMenu(GLUT_MIDDLE_BUTTON);
    }

    _attachedButton = MouseButton::Unknown;
}

void Menu::menuCallback(int option) {
    const auto menuId = glutGetMenu();

    auto it = menus.find(menuId);

    // 如果找不到對應的 Menu，直接返回
    if (it == menus.end()) {
        return;
    }

    auto* menu = it->second;

    if (!Window::canReceiveInput(menu->_windowId)) {
        Window::activateModalWindow();
        return;
    }

    auto itemIt = menu->_items.find(option);

    // 如果找不到對應的 MenuItem，直接返回
    if (itemIt != menu->_items.end()) {
        itemIt->second.action();
    }
}

}  // namespace paint::ui
