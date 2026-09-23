#include "shape.hpp"

#include <cmath>

namespace shape {

#pragma region Line

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

bool Line::onMouseDown(EventState& eventState) {
    start = eventState.mousePosition;
    end = eventState.mousePosition;

    glutPostRedisplay();

    return false;  // 不需要提交草稿
}

bool Line::onMouseUp(EventState& eventState) {
    end = eventState.mousePosition;

    glutPostRedisplay();

    return true;  // 提交草稿
}

bool Line::onMouseMove(EventState& eventState) {
    end = eventState.mousePosition;

    glutPostRedisplay();

    return false;
}

#pragma endregion  // Line

#pragma region Stroke

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

bool Stroke::onMouseDown(EventState& eventState) {
    addPoint(eventState.mousePosition);
    glutPostRedisplay();
    return false;
}

bool Stroke::onMouseUp(EventState& eventState) {
    addPoint(eventState.mousePosition);
    glutPostRedisplay();
    return true;
}

bool Stroke::onMouseMove(EventState& eventState) {
    addPoint(eventState.mousePosition);
    glutPostRedisplay();
    return false;
}

#pragma endregion  // Stroke

#pragma region Rectangle

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

bool Rectangle::onMouseDown(EventState& eventState) {
    start = eventState.mousePosition;
    end = eventState.mousePosition;

    glutPostRedisplay();

    return false;  // 不需要提交草稿
}

bool Rectangle::onMouseUp(EventState& eventState) {
    end = eventState.mousePosition;

    glutPostRedisplay();

    return true;  // 提交草稿
}

bool Rectangle::onMouseMove(EventState& eventState) {
    end = eventState.mousePosition;

    glutPostRedisplay();

    return false;
}

#pragma endregion  // Rectangle

// void Ellipse::update(Point point) {
//     if (point != end) {
//         end = point;
//     }
// }

// void Polygon::update(Point point) {
//     addPoint(point);
// }

}  // namespace shape
