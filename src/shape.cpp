#include "shape.hpp"

#include <algorithm>
#include <cmath>

namespace shape {

void Line::draw() const {
    glColor4fv(color);

    const GLfloat dx = end.getX() - start.getX();
    const GLfloat dy = end.getY() - start.getY();
    const GLfloat length = std::sqrt(dx * dx + dy * dy);

    // 跳過繪製長度為零的線段
    if (length == 0.0f) {
        return;
    }

    // 計算法線向量，並將其縮放到線寬的一半
    const GLfloat nx = 0.5f * width * dy / length;
    const GLfloat ny = -0.5f * width * dx / length;

    glBegin(GL_QUADS);
    glVertex2f(start.getX() + nx, start.getY() + ny);
    glVertex2f(start.getX() - nx, start.getY() - ny);
    glVertex2f(end.getX() - nx, end.getY() - ny);
    glVertex2f(end.getX() + nx, end.getY() + ny);
    glEnd();
}

void Stroke::draw() const {
    if (points.empty()) {
        return;
    }

    glColor4fv(color);

    Point prevPoint = points.front();

    glBegin(GL_QUADS);

    for (const auto& point : points) {
        if (point == prevPoint) {
            continue;  // 跳過與前一個點相同的點，避免繪製長度為零的線段
        }

        const GLfloat dx = point.getX() - prevPoint.getX();
        const GLfloat dy = point.getY() - prevPoint.getY();
        const GLfloat length = std::sqrt(dx * dx + dy * dy);

        const GLfloat nx = 0.5f * width * dy / length;
        const GLfloat ny = -0.5f * width * dx / length;

        glVertex2f(prevPoint.getX() + nx, prevPoint.getY() + ny);
        glVertex2f(prevPoint.getX() - nx, prevPoint.getY() - ny);
        glVertex2f(point.getX() - nx, point.getY() - ny);
        glVertex2f(point.getX() + nx, point.getY() + ny);

        prevPoint = point;
    }

    glEnd();
}

void Rectangle::draw() const {
    GLfloat leftX = std::min(start.getX(), end.getX());
    GLfloat rightX = std::max(start.getX(), end.getX());
    GLfloat topY = std::min(start.getY(), end.getY());
    GLfloat bottomY = std::max(start.getY(), end.getY());

    glColor4fv(color);

    glBegin(GL_QUADS);

    // top edge
    glVertex2f(leftX, topY);
    glVertex2f(rightX, topY);
    glVertex2f(rightX, topY + width);
    glVertex2f(leftX, topY + width);

    // bottom edge
    glVertex2f(leftX, bottomY - width);
    glVertex2f(rightX, bottomY - width);
    glVertex2f(rightX, bottomY);
    glVertex2f(leftX, bottomY);

    // left edge
    glVertex2f(leftX, topY);
    glVertex2f(leftX + width, topY);
    glVertex2f(leftX + width, bottomY);
    glVertex2f(leftX, bottomY);

    // right edge
    glVertex2f(rightX - width, topY);
    glVertex2f(rightX, topY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX - width, bottomY);

    // fill
    if (fillColor[3] > 0.0f && (rightX - leftX > 2 * width) && (bottomY - topY > 2 * width)) {
        glColor4fv(fillColor);
        glVertex2f(leftX + width, topY + width);
        glVertex2f(rightX - width, topY + width);
        glVertex2f(rightX - width, bottomY - width);
        glVertex2f(leftX + width, bottomY - width);
    }

    glEnd();
}

void Polygon::draw() const {
    if (points.size() < 2) return;

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
    glColor4fv(color);
    glLineWidth(width);

    glBegin(GL_LINE_LOOP);
    for (const auto& p : points) {
        glVertex2f(p.getX(), p.getY());
    }
    glEnd();

    glLineWidth(1.0f);  // 恢復，避免影響其他繪圖
}

}  // namespace shape
