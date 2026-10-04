#pragma once

#include "common/color.hpp"

namespace paint::render {

class RenderContext {
   public:
    ~RenderContext() = default;

    void pushTransform();  // 保存目前的座標轉換狀態
    void popTransform();   // 恢復上一個座標轉換狀態

    void translate(float x, float y);  // 平移目前座標系
    void fillRect(int width, int height, const ColorRGBA& color);
};

}  // namespace paint::render
