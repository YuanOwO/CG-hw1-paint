#pragma once

#include <cmath>

const float PI = static_cast<float>(M_PI);
const float EPSILON = 1e-6f;  // 用於浮點數比較的容差值

// 點座標，必要時也可以用於表示向量（例如兩點之間的向量）。
class Point {
   public:
    Point() : x(0.0f), y(0.0f) {}
    Point(float xCoord, float yCoord) : x(xCoord), y(yCoord) {}

    void setX(float xCoord) { x = xCoord; }
    float getX() const { return x; }

    void setY(float yCoord) { y = yCoord; }
    float getY() const { return y; }

    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    bool operator!=(const Point& other) const { return !(*this == other); }

    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    Point operator-(const Point& other) const { return Point(x - other.x, y - other.y); }
    Point operator*(float scalar) const { return Point(x * scalar, y * scalar); }
    Point operator/(float scalar) const { return Point(x / scalar, y / scalar); }

   protected:
    float x, y;
};

inline Point operator*(float scalar, const Point& vec) {
    return vec * scalar;
}

using Vector = Point;  // 將 Point 當作向量使用，方便表示兩點之間的向量。

inline float abs(const Vector& vec) {
    return std::hypot(vec.getX(), vec.getY());
}

inline float dot(const Vector& a, const Vector& b) {
    return a.getX() * b.getX() + a.getY() * b.getY();
}

inline float cross(const Vector& a, const Vector& b) {
    return a.getX() * b.getY() - a.getY() * b.getX();
}

inline Vector normalize(const Vector& vec) {
    float length = abs(vec);
    if (length < EPSILON) {
        return Vector(0.0f, 0.0f);
    }
    return vec * (1.0f / length);
}

inline Vector perpendicular(const Vector& vec) {
    // 返回垂直於 vec 的向量，順時針旋轉 90 度
    return Vector(-vec.getY(), vec.getX());
}

// 求直線 AB 與 CD 的交點；平行或共線時回傳 false
inline bool lineInter(const Point& a, const Point& b, const Point& c, const Point& d, Point& result) {
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
