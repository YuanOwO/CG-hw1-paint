#pragma once

#include <array>

#include "common/color.hpp"

namespace paint::palette {

// 顏色選擇器與其他繪圖介面共用的基本色票。
inline constexpr std::array<Color, 16> Basic{
    Color::Black,   Color::White,  Color::Gray,    Color::Red,
    Color::Orange,  Color::Yellow, Color::Green,   Color::Lime,
    Color::Cyan,    Color::Blue,   Color::Navy,    Color::Purple,
    Color::Magenta, Color::Pink,   Color::Brown,   Color::Teal,
};

}  // namespace paint::palette
