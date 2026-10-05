#pragma once

#include "common/color.hpp"
#include "common/font.hpp"

namespace paint::ui::theme {

// UI 字體；繪圖內容使用的字型不屬於主題設定。
inline const FontStyle HeadingFont = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18};
inline const FontStyle BodyFont = GfntFontStyle{GfntFontId::CUBIC_11};

// 視窗與文字
inline const ColorRGBA WindowBackground{0.97f, 0.98f, 0.99f};
inline const ColorRGBA Text{0.12f, 0.16f, 0.23f};
inline const ColorRGBA ButtonText{0.22f, 0.27f, 0.34f};
inline const ColorRGBA MutedText{0.36f, 0.40f, 0.47f};
inline const ColorRGBA ErrorText{0.78f, 0.16f, 0.16f};

// 一般控制項
inline const ColorRGBA ControlBackground{Color::White};
inline const ColorRGBA ControlHovered{0.94f, 0.95f, 0.97f};
inline const ColorRGBA ControlPressed{0.87f, 0.89f, 0.93f};
inline const ColorRGBA ControlBorder{0.80f, 0.83f, 0.88f};
inline const ColorRGBA InputBorder{0.68f, 0.71f, 0.77f};

// 主要操作
inline const ColorRGBA Primary{0.15f, 0.36f, 0.85f};
inline const ColorRGBA PrimaryHovered{0.12f, 0.31f, 0.76f};
inline const ColorRGBA PrimaryPressed{0.10f, 0.25f, 0.64f};

}  // namespace paint::ui::theme
