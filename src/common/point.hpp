#pragma once

#include <cmath>

namespace paint {

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

float abs(const Vector& vec);

float dot(const Vector& a, const Vector& b);

float cross(const Vector& a, const Vector& b);

Vector normalize(const Vector& vec);

Vector perpendicular(const Vector& vec);

// 求直線 AB 與 CD 的交點；平行或共線時回傳 false
bool lineInter(const Point& a, const Point& b, const Point& c, const Point& d, Point& result);

}  // namespace paint
