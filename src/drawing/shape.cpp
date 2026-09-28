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

void Path::addPoint(const Point& p, const bool force) {
    if (points.empty()) {
        points.push_back(p);
        return;
    }

    // 避免筆刷的點太密集，導致繪製出來的線條過於粗糙。
    const bool isTooClose = abs(points.back() - p) < std::max(style.stroke.width * 0.2f, 1.0f);

    if (force && isTooClose && points.size() >= 2) {
        // 強制加入點時，若太接近前一個點，則將前一個點移除，避免重疊。
        points.pop_back();
    }

    if (force || !isTooClose) {
        points.push_back(p);
    }
}

std::vector<Point> Polygon::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

void Polygon::addPoint(const Point& point) {
    points.push_back(point);
}

void Polygon::setLastPoint(const Point& point) {
    if (!points.empty()) {
        points.back() = point;
    }
}

void Polygon::removeLastPoint() {
    if (!points.empty()) {
        points.pop_back();
    }
}

}  // namespace paint::drawing
