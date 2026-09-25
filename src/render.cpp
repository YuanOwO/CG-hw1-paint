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

VertexJoinResult computeVertexJoin(const Point& a, const Point& b, const Point& c,
                                   const shape::StrokeStyle& style) {
    VertexJoinResult result{};
    result.type = shape::LineJoin::NONE;
    result.vertex = b;

    const GLfloat h = style.width * 0.5f;

    Vector ab = b - a;
    Vector bc = c - b;

    const GLfloat lenAB = abs(ab);
    const GLfloat lenBC = abs(bc);

    const bool zeroAB = lenAB <= EPSILON;
    const bool zeroBC = lenBC <= EPSILON;

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

    // 一般情況
    const auto turn = cross(u, v);
    const auto forward = dot(u, v);

    // 幾乎沒有轉向
    if (std::abs(turn) <= EPSILON) {
        if (forward > 0.0f) {
            // 同方向，不需要 join。
            result.type = shape::LineJoin::NONE;

            // n0、n1 理論上幾乎相同，用平均可以減少一點浮點誤差。
            result.inLeft = result.outLeft = (result.inLeft + result.outLeft) * 0.5f;
            result.inRight = result.outRight = (result.inRight + result.outRight) * 0.5f;
        } else {
            // 180° 折返
            result.type =
                style.join == shape::LineJoin::ROUND ? shape::LineJoin::ROUND : shape::LineJoin::BEVEL;

            result.inner = b;

            // 此時 outer/inner 的概念其實退化了，
            // 先留下各線段自己的 offset endpoint。
            result.outer0 = result.inLeft;
            result.outer1 = result.inRight;
        }

        return result;
    }

    // 左右兩側 offset line 分別求交點
    Point leftInter, rightInter;

    bool hasLeftInter = lineInter(a - h * n0, b - h * n0, b - h * n1, c - h * n1, leftInter);
    bool hasRightInter = lineInter(a + h * n0, b + h * n0, b + h * n1, c + h * n1, rightInter);

    // 理論上非平行的兩條 segment，其左右 offset line 都應該可以求交點
    // 但仍然保留 fallback，避免數值不穩定。
    if (!hasLeftInter || !hasRightInter) {
        result.type = style.join == shape::LineJoin::ROUND ? shape::LineJoin::ROUND : shape::LineJoin::BEVEL;

        result.inner = b;

        if (turn > 0.0f) {
            // screen-space 右轉
            // 左邊是 outer
            result.outer0 = result.inLeft;
            result.outer1 = result.outLeft;
        } else {
            // screen-space 左轉
            // 右邊是 outer
            result.outer0 = result.inRight;
            result.outer1 = result.outRight;
        }

        return result;
    }

    //
    // x→、y↓ 時：
    //
    // cross > 0 = 畫面上的右轉
    // cross < 0 = 畫面上的左轉
    //
    const bool turnRight = turn > 0.0f;

    Point innerInter;
    Point outerInter;

    Point outer0;
    Point outer1;

    if (turnRight) {
        // 右轉
        innerInter = rightInter;
        outerInter = leftInter;
        outer0 = result.inLeft;
        outer1 = result.outLeft;
    } else {
        // 左轉
        innerInter = leftInter;
        outerInter = rightInter;
        outer0 = result.inRight;
        outer1 = result.outRight;
    }

    // Miter
    const GLfloat outerDistance = abs(outerInter - b);

    const bool validMiter = style.join == shape::LineJoin::MITER && outerDistance <= h * style.miterLimit;

    if (validMiter) {
        result.type = shape::LineJoin::MITER;

        result.inLeft = result.outLeft = leftInter;
        result.inRight = result.outRight = rightInter;

        return result;
    }

    // Bevel / Round
    result.type = style.join == shape::LineJoin::ROUND ? shape::LineJoin::ROUND : shape::LineJoin::BEVEL;

    result.outer0 = outer0;
    result.outer1 = outer1;

    // inner intersection 也可能因接近 180° 而跑非常遠。
    // 尤其筆刷的 segment 很短時，不應該直接相信交點。
    const GLfloat innerDistance = abs(innerInter - b);

    // 一個偏保守的限制：
    //
    // 至少允許 2 * halfWidth，
    // 但也會參考附近 segment 的長度。
    //
    const GLfloat maxInnerDistance = std::max(h * 2.0f, std::min(lenAB, lenBC) + h);

    if (innerDistance <= maxInnerDistance) {
        result.inner = innerInter;
    } else {
        // 接近折返或短 segment：
        // 不讓 inner 跑到幾十、幾百 px 外。
        result.inner = b;
    }

    if (turnRight) {
        // 右側 inner
        result.inRight = result.outRight = result.inner;
        result.inLeft = outer0;
        result.outLeft = outer1;
    } else {
        // 左側 inner
        result.inLeft = result.outLeft = result.inner;
        result.inRight = outer0;
        result.outRight = outer1;
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
