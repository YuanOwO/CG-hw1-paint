#include "common/point.hpp"

namespace paint {

float abs(const Vector& vec) {
    return std::hypot(vec.getX(), vec.getY());
}

float dot(const Vector& a, const Vector& b) {
    return a.getX() * b.getX() + a.getY() * b.getY();
}

float cross(const Vector& a, const Vector& b) {
    return a.getX() * b.getY() - a.getY() * b.getX();
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
    return Vector(-vec.getY(), vec.getX());
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
