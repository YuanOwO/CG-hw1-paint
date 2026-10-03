#include "drawing/shape.hpp"

#include <algorithm>
#include <cmath>

namespace paint::drawing {

std::vector<Point> PointShape::getVertices() const {
    std::vector<Point> vertices = {position};
    return vertices;
}

std::vector<Point> Line::getVertices() const {
    std::vector<Point> vertices = {_start, _end};
    return vertices;
}

std::vector<Point> Rectangle::getVertices() const {
    std::vector<Point> vertices;

    const float left = std::min(_start.x(), _end.x());
    const float right = std::max(_start.x(), _end.x());
    const float top = std::min(_start.y(), _end.y());
    const float bottom = std::max(_start.y(), _end.y());

    vertices.emplace_back(left, top);
    vertices.emplace_back(left, bottom);
    vertices.emplace_back(right, bottom);
    vertices.emplace_back(right, top);

    return vertices;
}

std::vector<Point> Ellipse::getVertices() const {
    std::vector<Point> vertices;

    const float cx = (_start.x() + _end.x()) / 2.0f;
    const float cy = (_start.y() + _end.y()) / 2.0f;
    const float rx = std::abs(_end.x() - _start.x()) / 2.0f;
    const float ry = std::abs(_end.y() - _start.y()) / 2.0f;

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
    const bool isTooClose = abs(points.back() - p) < std::max(_style.strokeWidth() * 0.2f, 1.0f);

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
