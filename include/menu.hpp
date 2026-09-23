#pragma once

#include <unordered_map>

#include "color.hpp"

namespace menu {

enum class Menu_Width {
    WIDTH_1 = 1,
    WIDTH_3 = 3,
    WIDTH_5 = 5,
    WIDTH_10 = 10,
    WIDTH_25 = 25,
    WIDTH_50 = 50,
    WIDTH_THICKER,
    WIDTH_THICKERRR,
    WIDTH_THICKERRRRR,
    WIDTH_THINNER,
    WIDTH_THINNERRR,
    WIDTH_THINNERRRRR,
};

enum class Menu_Main {
    MENU_CLEAR,
    MENU_QUIT,
};

void init();

}  // namespace menu
