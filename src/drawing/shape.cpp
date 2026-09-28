#include "drawing/shape.hpp"

#include <algorithm>
#include <cmath>

namespace paint::drawing {

std::vector<Point> Line::getVertices() const {
    std::vector<Point> vertices = {start, end};
    return vertices;
}

std::vector<Point> Rectangle::getVertices() const {
    std::vector<Point> vertices;

    const float left = std::min(start.getX(), end.getX());
    const float right = std::max(start.getX(), end.getX());
    const float top = std::min(start.getY(), end.getY());
    const float bottom = std::max(start.getY(), end.getY());

    vertices.emplace_back(left, top);
    vertices.emplace_back(left, bottom);
    vertices.emplace_back(right, bottom);
    vertices.emplace_back(right, top);

    return vertices;
}

std::vector<Point> Ellipse::getVertices() const {
    std::vector<Point> vertices;

    const float cx = (start.getX() + end.getX()) / 2.0f;
    const float cy = (start.getY() + end.getY()) / 2.0f;
    const float rx = std::abs(end.getX() - start.getX()) / 2.0f;
    const float ry = std::abs(end.getY() - start.getY()) / 2.0f;

    if (rx == 0.0f || ry == 0.0f) {
        vertices.emplace_back(cx, cy);  // 如果橢圓的半徑為零，則只繪製中心點
        return vertices;
    }

    const int segments = 64;

    for (int i = 0; i < segments; i++) {
        const float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        vertices.emplace_back(cx + rx * std::cos(angle), cy + ry * std::sin(angle));
    }

    return vertices;
}

std::vector<Point> Path::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

std::vector<Point> Polygon::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

}  // namespace paint::drawing
