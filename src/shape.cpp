#include "shape.hpp"

#include <algorithm>
#include <cmath>

namespace shape {
namespace {

void drawDot(const Point& point, GLfloat radius, const GLfloat* color) {
    glColor4fv(color);

    int numSegments = 32;  // 可以調整以改變圓的平滑度
    GLfloat ox = point.getX(), oy = point.getY();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ox, oy);  // 圓心

    for (int i = 0; i <= numSegments; ++i) {
        GLfloat angle = 2.0f * M_PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(numSegments);
        GLfloat x = ox + radius * std::cos(angle);
        GLfloat y = oy + radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawLine(const Point& p1, const Point& p2, GLfloat width, const GLfloat* color) {
    glColor4fv(color);

    const GLfloat dx = p2.getX() - p1.getX();
    const GLfloat dy = p2.getY() - p1.getY();
    const GLfloat length = std::sqrt(dx * dx + dy * dy);

    // 跳過繪製長度為零的線段
    if (length == 0.0f) {
        return;
    }

    // 計算法線向量，並將其縮放到線寬的一半
    const GLfloat nx = 0.5f * width * dy / length;
    const GLfloat ny = -0.5f * width * dx / length;

    glBegin(GL_QUADS);
    glVertex2f(p1.getX() + nx, p1.getY() + ny);
    glVertex2f(p1.getX() - nx, p1.getY() - ny);
    glVertex2f(p2.getX() - nx, p2.getY() - ny);
    glVertex2f(p2.getX() + nx, p2.getY() + ny);
    glEnd();
}

}  // namespace

void Line::draw() const {
    drawLine(start, end, width, color);
}

void Stroke::draw() const {
    if (points.empty()) {
        return;
    }

    GLfloat dotRadius = 0.5f * width;
    Point prevPoint = points.front();
    drawDot(prevPoint, dotRadius, color);

    for (const auto& point : points) {
        if (point == prevPoint) {
            continue;  // 跳過與前一個點相同的點，避免繪製長度為零的線段
        }
        drawLine(prevPoint, point, width, color);
        drawDot(point, dotRadius, color);
        prevPoint = point;
    }
}

void Rectangle::draw() const {
    const GLfloat left = std::min(start.getX(), end.getX());
    const GLfloat right = std::max(start.getX(), end.getX());
    const GLfloat top = std::min(start.getY(), end.getY());
    const GLfloat bottom = std::max(start.getY(), end.getY());

    if (left == right || top == bottom) return;

    glColor4fv(color);

    // 邊框已占滿矩形，直接畫整塊。
    if (right - left <= 2 * width || bottom - top <= 2 * width) {
        glRectf(left, top, right, bottom);
        return;
    }

    const GLfloat innerLeft = left + width;
    const GLfloat innerRight = right - width;
    const GLfloat innerTop = top + width;
    const GLfloat innerBottom = bottom - width;

    // 四個不重疊的邊框區域。
    glRectf(left, top, right, innerTop);
    glRectf(left, innerBottom, right, bottom);
    glRectf(left, innerTop, innerLeft, innerBottom);
    glRectf(innerRight, innerTop, right, innerBottom);

    // 內部填色：透明就跳過
    if (fillColor[3] > 0.0f) {
        glColor4fv(fillColor);
        glRectf(innerLeft, innerTop, innerRight, innerBottom);
    }
}

void Polygon::draw() const {
    GLfloat dotRadius = 0.5f * width;

    if (points.size() < 2) {
        drawDot(points.front(), dotRadius, color);
        return;
    }

    // 內部填色：透明就跳過
    if (points.size() >= 3 && fillColor[3] > 0.0f) {
        glColor4fv(fillColor);

        glBegin(GL_POLYGON);
        for (const auto& p : points) {
            glVertex2f(p.getX(), p.getY());
        }
        glEnd();
    }

    // 邊框：最後一個頂點會自動連回第一個
    Point prevPoint = points.front();
    drawLine(prevPoint, points.back(), width, color);
    drawDot(prevPoint, dotRadius, color);

    for (const auto& point : points) {
        drawLine(prevPoint, point, width, color);
        drawDot(point, dotRadius, color);
        prevPoint = point;
    }
}

}  // namespace shape
