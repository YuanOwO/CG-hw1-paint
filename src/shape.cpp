#include "shape.hpp"

#include <algorithm>
#include <cmath>

#include "render.hpp"

namespace shape {

std::vector<Point> Line::getVertices() const {
    std::vector<Point> vertices = {start, end};
    return vertices;
}

std::vector<Point> Rectangle::getVertices() const {
    std::vector<Point> vertices;

    const GLfloat left = std::min(start.getX(), end.getX());
    const GLfloat right = std::max(start.getX(), end.getX());
    const GLfloat top = std::min(start.getY(), end.getY());
    const GLfloat bottom = std::max(start.getY(), end.getY());

    vertices.emplace_back(left, top);
    vertices.emplace_back(left, bottom);
    vertices.emplace_back(right, bottom);
    vertices.emplace_back(right, top);

    return vertices;
}

std::vector<Point> Ellipse::getVertices() const {
    std::vector<Point> vertices;

    const GLfloat cx = (start.getX() + end.getX()) / 2.0f;
    const GLfloat cy = (start.getY() + end.getY()) / 2.0f;
    const GLfloat rx = std::abs(end.getX() - start.getX()) / 2.0f;
    const GLfloat ry = std::abs(end.getY() - start.getY()) / 2.0f;

    if (rx == 0.0f || ry == 0.0f) {
        vertices.emplace_back(cx, cy);  // 如果橢圓的半徑為零，則只繪製中心點
        return vertices;
    }

    const int segments = 64;

    for (int i = 0; i < segments; i++) {
        const GLfloat angle = 2.0f * PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(segments);
        vertices.emplace_back(cx + rx * std::cos(angle), cy + ry * std::sin(angle));
    }

    return vertices;
}

std::vector<Point> Stroke::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

std::vector<Point> Polygon::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

}  // namespace shape
