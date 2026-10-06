#pragma once

#include "common/color.hpp"

namespace paint::drawing {

// 物件的顏色設定，形狀與文字共用。
struct PaintStyle {
    ColorRGBA color = Color::Black;            // 主要顏色：Stroke、Point、Filled 與文字共用
    ColorRGBA fillColor = Color::Transparent;  // 填入顏色：只有 Advanced 模式會使用
};

}  // namespace paint::drawing
