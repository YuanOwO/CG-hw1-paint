#include "common/point.hpp"

#include <algorithm>

namespace paint {

float abs(const Vector& vec) {
    return std::hypot(vec.x(), vec.y());
}

float dot(const Vector& a, const Vector& b) {
    return a.x() * b.x() + a.y() * b.y();
}

float cross(const Vector& a, const Vector& b) {
    return a.x() * b.y() - a.y() * b.x();
}

Vector normalize(const Vector& vec) {
    float length = abs(vec);
    if (length < EPSILON) {
        return Vector(0.0f, 0.0f);
    }
    return vec * (1.0f / length);
}

Vector perpendicular(const Vector& vec) {
    // 返回垂直於 vec 的向量，順時針旋轉 90 度
    return Vector(-vec.y(), vec.x());
}

float distanceToSegment(const Point& point, const Point& start, const Point& end) {
    const Vector segment = end - start;
    const float lengthSquared = dot(segment, segment);

    if (lengthSquared <= EPSILON) {
        return abs(point - start);
    }

    const float amount = std::clamp(dot(point - start, segment) / lengthSquared, 0.0f, 1.0f);
    return abs(point - (start + segment * amount));
}

bool pointInPolygon(const Point& point, const std::vector<Point>& vertices) {
    if (vertices.size() < 3) {
        return false;
    }

    bool inside = false;

    for (std::size_t i = 0, previous = vertices.size() - 1; i < vertices.size(); previous = i++) {
        const Point& a = vertices[i];
        const Point& b = vertices[previous];
        const bool crosses = (a.y() > point.y()) != (b.y() > point.y());

        if (crosses) {
            const float crossingX = (b.x() - a.x()) * (point.y() - a.y()) / (b.y() - a.y()) + a.x();
            if (point.x() < crossingX) {
                inside = !inside;
            }
        }
    }

    return inside;
}

// 求直線 AB 與 CD 的交點；平行或共線時回傳 false
bool lineInter(const Point& a, const Point& b, const Point& c, const Point& d, Point& result) {
    Vector ab = b - a;
    Vector cd = d - c;

    auto _cross = cross(ab, cd);
    auto _scale = abs(ab) * abs(cd);

    if (_scale < EPSILON || std::abs(_cross) <= EPSILON * _scale) return false;

    Vector ac = c - a;
    auto t = cross(ac, cd) / _cross;

    result = a + ab * t;

    return true;
}

}  // namespace paint
