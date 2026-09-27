#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shapeStyle.hpp"

namespace paint::drawing {

// 填滿封閉區域
void fill(const std::vector<Point>& vertices, const FillStyle& style);

// 描邊
void stroke(const std::vector<Point>& vertices, const bool isClosed, const StrokeStyle& style);

// 同時處理 fill + stroke
void drawww(std::vector<Point>& vertices, const bool isClosed, const ShapeStyle& style);

}  // namespace paint::drawing
