#pragma once

namespace paint {

class RenderContext {
   public:
    ~RenderContext() = default;

    void pushTransform();  // 保存目前的座標轉換狀態
    void popTransform();   // 恢復上一個座標轉換狀態

    void translate(float x, float y);  // 平移目前座標系
};

}  // namespace paint
