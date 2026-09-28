#include "render/renderer.hpp"

#include <GL/freeglut.h>

#include <cmath>
#include <utility>

#include "drawing/shape.hpp"

using paint::drawing::FillStyle;
using paint::drawing::LineCap;
using paint::drawing::LineJoin;
using paint::drawing::ShapeStyle;
using paint::drawing::StrokeStyle;

namespace paint {

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

void triangle(const Point& a, const Point& b, const Point& c) {
    glBegin(GL_TRIANGLES);
    glVertex2f(a.getX(), a.getY());
    glVertex2f(b.getX(), b.getY());
    glVertex2f(c.getX(), c.getY());
    glEnd();
}

struct StrokeVertexResult {
    bool hasIn, hasOut;

    LineJoin joinType = LineJoin::NONE;  // 使用的連接類型（MITER、BEVEL、ROUND）
    LineCap capType = LineCap::BUTT;     // 使用的端點類型（BUTT、SQUARE、ROUND）

    Point vertex;  // 連接點的座標（B 點）

    Point inLeft, inRight;        // AB 線身到達 B 時的左右邊界端點
    Point outLeft, outRight;      // BC 線身離開 B 時的左右邊界端點
    Point inner, outer0, outer1;  // bevel 三角形的三個頂點

    float turn;  // AB 與 BC 的轉向，方便畫圓角時使用
};

StrokeVertexResult computeStrokeVertex(const Point& prev, const Point& curr, const Point& next,
                                       const StrokeStyle& style) {
    StrokeVertexResult result{};
    result.hasIn = prev != curr;
    result.hasOut = curr != next;
    result.joinType = LineJoin::NONE;
    result.capType = style.cap;
    result.vertex = curr;

    // 沒有方向時，截面退化在 B，避免留下原點座標。
    result.inLeft = result.inRight = curr;
    result.outLeft = result.outRight = curr;
    result.inner = result.outer0 = result.outer1 = curr;

    const GLfloat h = style.width * 0.5f;

    Vector ab = curr - prev;
    Vector bc = next - curr;

    if (!result.hasIn && !result.hasOut) {
        return result;
    }

    const GLfloat lenAB = abs(ab);
    const GLfloat lenBC = abs(bc);

    const Vector u = !result.hasIn ? Vector{} : ab / lenAB;
    const Vector v = !result.hasOut ? Vector{} : bc / lenBC;
    const Vector n0 = perpendicular(u);
    const Vector n1 = perpendicular(v);

    // 預設保留前後線段各自的截面。
    result.inLeft = curr - h * n0;
    result.inRight = curr + h * n0;
    result.outLeft = curr - h * n1;
    result.outRight = curr + h * n1;

    // 開放路徑端點：只有一個有效方向，不計算接角。
    if (!result.hasIn || !result.hasOut) {
        // SQUARE 端點需要額外延伸截面。
        if (!result.hasIn && result.capType == LineCap::SQUARE) {
            result.outLeft = result.outLeft - h * v;
            result.outRight = result.outRight - h * v;
        }

        if (!result.hasOut && result.capType == LineCap::SQUARE) {
            result.inLeft = result.inLeft + h * u;
            result.inRight = result.inRight + h * u;
        }

        return result;
    }

    // 一般情況
    const auto turn = cross(u, v);
    const auto forward = dot(u, v);

    result.turn = turn;

    // 幾乎沒有轉向
    if (std::abs(turn) <= EPSILON) {
        if (forward > 0.0f) {
            // 同方向，不需要 join。
            result.joinType = LineJoin::NONE;

            // n0、n1 理論上幾乎相同，用平均可以減少一點浮點誤差。
            result.inLeft = result.outLeft = (result.inLeft + result.outLeft) * 0.5f;
            result.inRight = result.outRight = (result.inRight + result.outRight) * 0.5f;
        } else {
            // 180° 折返
            result.joinType = style.join == LineJoin::ROUND ? LineJoin::ROUND : LineJoin::BEVEL;

            result.inner = curr;

            // 此時 outer/inner 的概念其實退化了，
            // 先留下各線段自己的 offset endpoint。
            result.outer0 = result.inLeft;
            result.outer1 = result.inRight;
        }

        return result;
    }

    // 左右兩側 offset line 分別求交點
    Point leftInter, rightInter;

    bool hasLeftInter = lineInter(prev - h * n0, curr - h * n0, curr - h * n1, next - h * n1, leftInter);
    bool hasRightInter = lineInter(prev + h * n0, curr + h * n0, curr + h * n1, next + h * n1, rightInter);

    // 理論上非平行的兩條 segment，其左右 offset line 都應該可以求交點
    // 但仍然保留 fallback，避免數值不穩定。
    if (!hasLeftInter || !hasRightInter) {
        result.joinType = style.join == LineJoin::ROUND ? LineJoin::ROUND : LineJoin::BEVEL;

        result.inner = curr;

        if (turn > 0.0f) {
            result.outer0 = result.inLeft;
            result.outer1 = result.outLeft;
        } else {
            result.outer0 = result.inRight;
            result.outer1 = result.outRight;
        }

        return result;
    }

    const bool turnRight = turn > 0.0f;

    Point innerInter;
    Point outerInter;

    Point outer0;
    Point outer1;

    if (turnRight) {
        innerInter = rightInter;
        outerInter = leftInter;
        outer0 = result.inLeft;
        outer1 = result.outLeft;
    } else {
        innerInter = leftInter;
        outerInter = rightInter;
        outer0 = result.inRight;
        outer1 = result.outRight;
    }

    // Miter
    const GLfloat outerDistance = abs(outerInter - curr);

    // 檢查是否符合 Miter 連接的條件
    const bool validMiter = style.join == LineJoin::MITER && outerDistance <= h * style.miterLimit;

    if (validMiter) {
        result.joinType = LineJoin::MITER;

        result.inLeft = result.outLeft = leftInter;
        result.inRight = result.outRight = rightInter;

        return result;
    }

    // Bevel / Round
    result.joinType = style.join == LineJoin::ROUND ? LineJoin::ROUND : LineJoin::BEVEL;

    result.outer0 = outer0;
    result.outer1 = outer1;

    // inner intersection 也可能因接近 180° 而跑非常遠。
    // 尤其筆刷的 segment 很短時，不應該直接相信交點。
    const GLfloat innerDistance = abs(innerInter - curr);

    // 一個偏保守的限制：
    // 至少允許 2 * halfWidth，
    // 但也會參考附近 segment 的長度。
    const GLfloat maxInnerDistance = std::max(h * 2.0f, std::min(lenAB, lenBC) + h);

    if (innerDistance <= maxInnerDistance) {
        result.inner = innerInter;
    } else {
        // 接近折返或短 segment：
        // 不讓 inner 跑到幾十、幾百 px 外。
        result.inner = curr;
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

void drawRoundArc(const Point& center, const Point& start, const Point& end, const GLfloat radius,
                  const GLfloat turn) {
    const GLfloat angleStart = std::atan2(start.getY() - center.getY(), start.getX() - center.getX());
    const GLfloat angleEnd = std::atan2(end.getY() - center.getY(), end.getX() - center.getX());

    GLfloat angleDiff = angleEnd - angleStart;

    if (turn > 0.0f) {
        // 右轉，確保 angleDiff 為正
        if (angleDiff < 0.0f) {
            angleDiff += 2.0f * PI;
        }
    } else {
        // 左轉，確保 angleDiff 為負
        if (angleDiff > 0.0f) {
            angleDiff -= 2.0f * PI;
        }
    }

    // 根據弧長決定採樣數量，至少 4 個 segment
    const int segments = std::max(4, static_cast<int>(std::ceil(std::abs(angleDiff) / (PI / 16.0f))));

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(center.getX(), center.getY());
    for (int i = 0; i <= segments; ++i) {
        const GLfloat angle =
            angleStart + angleDiff * static_cast<GLfloat>(i) / static_cast<GLfloat>(segments);
        const GLfloat x = center.getX() + radius * std::cos(angle);
        const GLfloat y = center.getY() + radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

////////////////////////////////////////////////////////////////////////

void fill(const std::vector<Point>& vertices, const FillStyle& style) {
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

void stroke(const std::vector<Point>& vertices, const bool isClosed, const StrokeStyle& style) {
    const GLfloat width = style.width;
    const GLfloat color[] = {style.color.r, style.color.g, style.color.b, style.color.a};

    if (color[3] <= 0.0f || width <= 0.0f || vertices.empty()) return;

    glColor4fv(color);

    if (vertices.size() == 1) {
        switch (style.cap) {
        case LineCap::ROUND: {
            // 畫圓點， width = 1.0f 時，會退化成單個像素點。
            if (width > 1.0f) {
                drawRoundArc(vertices[0], vertices[0] + Point(width * 0.5f, 0.0f),
                             vertices[0] + Point(-width * 0.5f, 0.0f), width * 0.5f, 1.0f);
                drawRoundArc(vertices[0], vertices[0] + Point(width * 0.5f, 0.0f),
                             vertices[0] + Point(-width * 0.5f, 0.0f), width * 0.5f, -1.0f);
                break;
            }
        }
        case LineCap::SQUARE: {
            const Point p1 = vertices[0] + Point(width * 0.5f, width * 0.5f);
            const Point p2 = vertices[0] + Point(-width * 0.5f, width * 0.5f);
            const Point p3 = vertices[0] + Point(-width * 0.5f, -width * 0.5f);
            const Point p4 = vertices[0] + Point(width * 0.5f, -width * 0.5f);
            triangle(p1, p2, p3);
            triangle(p1, p3, p4);
            break;
        }
        case LineCap::BUTT:
        default:
            // BUTT cap 不需要額外繪製
            break;
        }
        return;
    }

    std::vector<StrokeVertexResult> info(vertices.size());

    // 計算每個頂點的 stroke 資訊
    for (int i = 0; i < vertices.size(); i++) {
        Point prev = vertices[i == 0 ? vertices.size() - 1 : i - 1];
        Point curr = vertices[i];
        Point next = vertices[i == vertices.size() - 1 ? 0 : i + 1];

        // 處理開放線段的起點和終點
        if (!isClosed) {
            if (i == 0) {
                prev = curr;
            } else if (i == vertices.size() - 1) {
                next = curr;
            }
        }

        info[i] = computeStrokeVertex(prev, curr, next, style);
    }

    // 繪製每個頂點與邊的 stroke
    for (int i = 0; i < info.size(); i++) {
        const auto& info0 = info[i];
        const auto& info1 = info[(i + 1) % info.size()];

        // 繪製邊
        if (info0.hasOut && info1.hasIn) {
            triangle(info0.outLeft, info0.outRight, info1.inRight);
            triangle(info0.outLeft, info1.inRight, info1.inLeft);
        }

        // 繪製 join 接角
        if (info0.hasIn && info0.hasOut) {
            if (info0.joinType == LineJoin::BEVEL) {
                triangle(info0.outer0, info0.outer1, info0.inner);
            } else if (info0.joinType == LineJoin::ROUND) {
                triangle(info0.inner, info0.vertex, info0.outer0);
                triangle(info0.inner, info0.outer1, info0.vertex);
                drawRoundArc(info0.vertex, info0.outer0, info0.outer1, width * 0.5f, info0.turn);
            }
        }

        // 3. 畫 cap 額外需要的 geometry
        if (info0.hasIn != info0.hasOut) {
            if (info0.capType == LineCap::ROUND) {
                if (info0.hasOut) {
                    // 起點：沿負角度方向，畫在線身後方。
                    drawRoundArc(info0.vertex, info0.outLeft, info0.outRight, width * 0.5f, -1.0f);
                } else {
                    // 終點：沿正角度方向，畫在線身前方。
                    drawRoundArc(info0.vertex, info0.inLeft, info0.inRight, width * 0.5f, 1.0f);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////

void basicPoint(const Point& position, const ShapeStyle& style) {
    glPointSize(style.pointSize);
    const GLfloat color[] = {style.stroke.color.r, style.stroke.color.g, style.stroke.color.b,
                             style.stroke.color.a};
    glColor4fv(color);
    glBegin(GL_POINTS);
    glVertex2f(position.getX(), position.getY());
    glEnd();
}

void basicDraw(const std::vector<Point>& vertices, const bool isClosed, const ShapeStyle& style) {
    const GLfloat width = style.stroke.width;
    const GLfloat color[] = {style.stroke.color.r, style.stroke.color.g, style.stroke.color.b,
                             style.stroke.color.a};

    if (color[3] <= 0.0f || width <= 0.0f || vertices.empty()) return;

    glColor4fv(color);
    glPointSize(width);
    glLineWidth(width);

    if (style.fillMode == drawing::FillMode::FILLED) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    int drawMode;

    if (vertices.size() == 1) {
        drawMode = GL_POINTS;
    } else if (vertices.size() == 2) {
        drawMode = GL_LINES;
    } else if (isClosed) {
        drawMode = GL_POLYGON;
    } else {
        drawMode = GL_LINE_STRIP;
    }

    glBegin(drawMode);

    for (const auto& v : vertices) {
        glVertex2f(v.getX(), v.getY());
    }

    glEnd();
}

}  // namespace

////////////////////////////////////////////////////////////////////////

void Renderer::draw(const drawing::Shape& shape) const {
    auto vertices = shape.getVertices();
    uniquefilter(vertices);

    const auto& style = shape.getStyle();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 先設定為填充模式

    // 如果是 PointShape，直接使用 basicPoint 繪製
    if (auto point = dynamic_cast<const drawing::PointShape*>(&shape)) {
        basicPoint(point->getPosition(), style);
        return;
    }

    // 根據 FillMode 決定使用哪種繪製方式
    if (style.fillMode != drawing::FillMode::ADVANCED) {
        // OUTLINE / FILLED 模式，直接使用基本繪製
        basicDraw(vertices, shape.isClosed(), style);
    } else {
        // ADVANCED 模式，使用自訂的填充與描邊方式
        if (shape.isClosed()) {
            fill(vertices, style.fill);
        }
        stroke(vertices, shape.isClosed(), style.stroke);
    }
}

}  // namespace paint
