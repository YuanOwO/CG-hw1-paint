#pragma once

#include <vector>

#include "point.hpp"
#include "shapeStyle.hpp"

namespace render {

// 填滿封閉區域
void fill(const std::vector<Point>& vertices, const shape::FillStyle& style);

// 描邊
void stroke(const std::vector<Point>& vertices, const bool isClosed, const shape::StrokeStyle& style);

// 同時處理 fill + stroke
void draw(std::vector<Point>& vertices, const bool isClosed, const shape::ShapeStyle& style);

}  // namespace render
