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

    // 以中點畫圓法繪製 1px 寬的圓周；圓心與半徑皆為整數像素。
    void strokeCircle(int centerX, int centerY, int radius, const ColorRGBA& color);

    // 填滿內外兩個中點圓之間的環形區域，包含兩個圓周本身；逐列填滿，不會留下空洞。
    void fillRing(int centerX, int centerY, int innerRadius, int outerRadius, const ColorRGBA& color);

    // 填滿左上角為 (x, y)、寬高為 width x height 像素的圓角矩形；圓角以中點畫圓法逐列計算。
    void fillRoundedRect(int x, int y, int width, int height, int radius, const ColorRGBA& color);
};

}  // namespace paint::render
