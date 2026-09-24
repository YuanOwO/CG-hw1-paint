#include "shape.hpp"

#include <algorithm>
#include <cmath>

const GLfloat PI = static_cast<GLfloat>(M_PI);

namespace shape {
namespace {

void drawDot(const Point& point, GLfloat radius, const GLfloat* color, bool rounded) {
    glColor4fv(color);

    if (rounded && radius >= 1.0f) {
        int numSegments = 32;  // 可以調整以改變圓的平滑度
        GLfloat ox = point.getX(), oy = point.getY();

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(ox, oy);  // 圓心

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

void drawLine(const Point& p1, const Point& p2, GLfloat width, const GLfloat* color) {
    glColor4fv(color);

    const GLfloat dx = p2.getX() - p1.getX();
    const GLfloat dy = p2.getY() - p1.getY();
    const GLfloat length = std::sqrt(dx * dx + dy * dy);

    // 跳過繪製長度為零的線段
    if (length == 0.0f) {
        return;
    }

    // 計算法向量，並將其縮放到線寬的一半
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

////////////////////////////////////////////////////////////////////////

void Shape::fillShape(const std::vector<Point>& vertices) const {
    if (fillColor[3] <= 0.0f) {
        return;  // 透明顏色不需要填充
    }

    if (vertices.size() < 3) {
        return;  // 至少需要三個頂點才能形成多邊形
    }

    glColor4fv(fillColor);
    glBegin(GL_POLYGON);
    for (const auto& v : vertices) {
        glVertex2f(v.getX(), v.getY());
    }
    glEnd();
}

void Shape::drawBorder(const std::vector<Point>& vertices) const {
    if (color[3] <= 0.0f) {
        return;  // 透明顏色不需要繪製邊框
    }

    GLfloat dotRadius = 0.5f * width;
    Point prevPoint = vertices.front();

    drawDot(prevPoint, dotRadius, color, isRoundedVertices());

    // 如果是封閉形狀，連接最後一個頂點和第一個頂點
    if (isClosed()) {
        drawLine(prevPoint, vertices.back(), width, color);
    }

    for (const auto& point : vertices) {
        if (point == prevPoint) {
            continue;  // 跳過與前一個點相同的點，避免繪製長度為零的線段
        }

        drawDot(point, dotRadius, color, isRoundedVertices());
        drawLine(prevPoint, point, width, color);
        prevPoint = point;
    }
}

////////////////////////////////////////////////////////////////////////

std::vector<Point> Line::getVertices() const {
    std::vector<Point> vertices = {start, end};
    return vertices;
}

std::vector<Point> Rectangle::getVertices() const {
    std::vector<Point> vertices;

    const GLfloat left = std::min(start.getX(), end.getX());
    const GLfloat right = std::max(start.getX(), end.getX());
    const GLfloat top = std::min(start.getY(), end.getY());
    const GLfloat bottom = std::max(start.getY(), end.getY());

    vertices.emplace_back(left, top);
    vertices.emplace_back(left, bottom);
    vertices.emplace_back(right, bottom);
    vertices.emplace_back(right, top);

    return vertices;
}

std::vector<Point> Ellipse::getVertices() const {
    std::vector<Point> vertices;

    const GLfloat cx = (start.getX() + end.getX()) / 2.0f;
    const GLfloat cy = (start.getY() + end.getY()) / 2.0f;
    const GLfloat rx = std::abs(end.getX() - start.getX()) / 2.0f;
    const GLfloat ry = std::abs(end.getY() - start.getY()) / 2.0f;

    if (rx == 0.0f || ry == 0.0f) {
        vertices.emplace_back(cx, cy);  // 如果橢圓的半徑為零，則只繪製中心點
        return vertices;
    }

    const int segments = 64;

    for (int i = 0; i <= segments; i++) {
        const GLfloat angle = 2.0f * PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(segments);
        vertices.emplace_back(cx + rx * std::cos(angle), cy + ry * std::sin(angle));
    }

    return vertices;
}

std::vector<Point> Stroke::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

std::vector<Point> Polygon::getVertices() const {
    std::vector<Point> vertices = points;  // 直接使用點的集合作為頂點
    return vertices;
}

}  // namespace shape
