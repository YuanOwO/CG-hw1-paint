#include "render.hpp"

#include <cmath>
#include <iostream>
#include <utility>

#include "shape.hpp"

namespace {
void uniquefilter(std::vector<Point>& vertices) {
    if (vertices.size() < 2) return;

    std::vector<Point> filtered;
    filtered.push_back(vertices.front());

    for (auto&& v : vertices) {
        if (v != filtered.back()) {
            filtered.push_back(v);
        }
    }

    vertices = std::move(filtered);
}
}  // namespace

namespace render {
namespace {

void triangle(const Point& a, const Point& b, const Point& c) {
    glBegin(GL_TRIANGLES);
    glVertex2f(a.getX(), a.getY());
    glVertex2f(b.getX(), b.getY());
    glVertex2f(c.getX(), c.getY());
    glEnd();
}

void drawDot(const Point& point, GLfloat radius, const GLfloat* color, shape::LineCap cap) {
    glColor4fv(color);

    if (cap == shape::LineCap::ROUND && radius >= 1.0f) {
        int numSegments = 32;  // 可以調整以改變圓的平滑度
        GLfloat ox = point.getX(), oy = point.getY();

        glBegin(GL_POLYGON);

        for (int i = 0; i <= numSegments; ++i) {
            GLfloat angle = 2.0f * PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(numSegments);
            GLfloat x = ox + radius * std::cos(angle);
            GLfloat y = oy + radius * std::sin(angle);
            glVertex2f(x, y);
        }

        glEnd();
    } else {
        glBegin(GL_QUADS);
        glVertex2f(point.getX() - radius, point.getY() - radius);
        glVertex2f(point.getX() + radius, point.getY() - radius);
        glVertex2f(point.getX() + radius, point.getY() + radius);
        glVertex2f(point.getX() - radius, point.getY() + radius);
        glEnd();
    }
}

struct VertexJoinResult {
    shape::LineJoin type;  // 使用的連接類型（MITER、BEVEL、ROUND）

    Point vertex;  // 連接點的座標（B 點）

    Point inLeft, inRight;        // AB 線身到達 B 時的左右邊界端點
    Point outLeft, outRight;      // BC 線身離開 B 時的左右邊界端點
    Point inner, outer0, outer1;  // bevel 三角形的三個頂點
};

VertexJoinResult computeVertexJoin(const Point& a, const Point& b, const Point& c, shape::StrokeStyle style) {
    auto h = style.width * 0.5f;

    Vector ab = b - a;
    Vector bc = c - b;

    auto lenAB = abs(ab);
    auto lenBC = abs(bc);

    bool zeroAB = lenAB <= EPSILON;
    bool zeroBC = lenBC <= EPSILON;

    VertexJoinResult result;
    result.type = shape::LineJoin::NONE;
    result.vertex = b;

    // 沒有方向時，截面退化在 B，避免留下原點座標。
    result.inLeft = result.inRight = b;
    result.outLeft = result.outRight = b;
    result.inner = result.outer0 = result.outer1 = b;

    if (zeroAB && zeroBC) {
        return result;
    }

    const Vector u = zeroAB ? Vector{} : ab / lenAB;
    const Vector v = zeroBC ? Vector{} : bc / lenBC;
    const Vector n0 = perpendicular(u);
    const Vector n1 = perpendicular(v);

    // 預設保留前後線段各自的截面。
    result.inLeft = b - h * n0;
    result.inRight = b + h * n0;
    result.outLeft = b - h * n1;
    result.outRight = b + h * n1;

    // 開放路徑端點：只有一個有效方向，不計算接角。
    if (zeroAB || zeroBC) return result;

    auto turn = cross(u, v);

    if (std::abs(turn) < EPSILON) {
        if (dot(u, v) > 0.0f) {
            // 同向直行
            result.inLeft = result.outLeft = b - h * n0;
            result.inRight = result.outRight = b + h * n0;
            result.type = shape::LineJoin::NONE;
            return result;
        } else {
            // 反向折返
            result.type = shape::LineJoin::BEVEL;
            return result;  // 之後可以另外設計 U-turn
        }
    }

    // 一般轉角，計算 miter / bevel / round。
    Vector bisector = normalize(n0 + n1);
    auto denom = dot(bisector, n0);

    // 避免除以零或非常小的數值，導致不穩定的結果
    if (std::abs(denom) < EPSILON) {
        denom = EPSILON * (denom < 0.0f ? -1.0f : 1.0f);
    }

    auto len = h / denom;

    bool miterValid = std::abs(len) <= h * style.miterLimit && std::abs(len) <= std::min(lenAB, lenBC) + h;

    // 如果 miter 不符資格就退化使用 bevel
    if (style.join == shape::LineJoin::MITER && miterValid) {
        result.type = shape::LineJoin::MITER;
        result.inLeft = b - len * bisector;
        result.inRight = b + len * bisector;
        result.outLeft = b - len * bisector;
        result.outRight = b + len * bisector;

        return result;
    }

    result.type = style.join == shape::LineJoin::ROUND ? shape::LineJoin::ROUND : shape::LineJoin::BEVEL;

    if (turn > 0) {
        result.inner = b + len * bisector;
        result.outer0 = b - h * n0;
        result.outer1 = b - h * n1;

        result.inRight = result.outRight = result.inner;
        result.inLeft = result.outer0;
        result.outLeft = result.outer1;

    } else {
        result.inner = b - len * bisector;
        result.outer0 = b + h * n0;
        result.outer1 = b + h * n1;

        result.inLeft = result.outLeft = result.inner;
        result.inRight = result.outer0;
        result.outRight = result.outer1;
    }

    return result;
}

}  // namespace

////////////////////////////////////////////////////////////////////////

void fill(const std::vector<Point>& vertices, const shape::FillStyle& style) {
    if (!style.enabled) return;

    const GLfloat fillColor[] = {style.color.r, style.color.g, style.color.b, style.color.a};

    if (fillColor[3] <= 0.0f || vertices.size() < 3) {
        return;  // 透明顏色或頂點數不足不需要填充
    }

    glColor4fv(fillColor);
    glBegin(GL_POLYGON);
    for (const auto& v : vertices) {
        glVertex2f(v.getX(), v.getY());
    }
    glEnd();
}

void stroke(const std::vector<Point>& vertices, const bool isClosed, const shape::StrokeStyle& style) {
    if (!style.enabled) return;

    const GLfloat width = style.width;
    const GLfloat color[] = {style.color.r, style.color.g, style.color.b, style.color.a};

    // 沿用原有 bevel 描邊；join、miterLimit 與線段端點樣式尚待實作。
    // vertices 已由呼叫端移除重複點。
    if (color[3] <= 0.0f || width <= 0.0f || vertices.empty()) return;

    if (vertices.size() == 1) {
        drawDot(vertices.front(), width * 0.5f, color, style.cap);
        return;
    }

    std::vector<VertexJoinResult> joins(vertices.size());

    for (int i = 0; i < vertices.size(); i++) {
        Point a = vertices[i == 0 ? vertices.size() - 1 : i - 1];
        Point b = vertices[i];
        Point c = vertices[i == vertices.size() - 1 ? 0 : i + 1];

        // 處理開放線段的起點和終點
        if (!isClosed || vertices.size() < 3) {
            if (i == 0) {
                a = b;
            } else if (i == vertices.size() - 1) {
                c = b;
            }
        }

        joins[i] = computeVertexJoin(a, b, c, style);
    }

    glColor4fv(color);

    for (int i = isClosed ? joins.size() - 1 : joins.size() - 2; i >= 0; i--) {
        const auto& join0 = joins[i];
        const auto& join1 = joins[(i + 1) % joins.size()];

        triangle(join0.outLeft, join0.outRight, join1.inRight);
        triangle(join0.outLeft, join1.inRight, join1.inLeft);
        if (join0.type == shape::LineJoin::BEVEL) {
            triangle(join0.outer0, join0.outer1, join0.inner);
        } else if (join0.type == shape::LineJoin::ROUND) {
            triangle(join0.inner, join0.vertex, join0.outer0);
            triangle(join0.inner, join0.outer1, join0.vertex);
            // TODO: 實作圓角連接
        }
    }
}

////////////////////////////////////////////////////////////////////////

void draw(std::vector<Point>& vertices, const bool isClosed, const shape::ShapeStyle& style) {
    uniquefilter(vertices);

    if (isClosed) {  // 封閉形狀才需要填滿
        fill(vertices, style.fill);
    }

    stroke(vertices, isClosed, style.stroke);
}

}  // namespace render
