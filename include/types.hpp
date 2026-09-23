#pragma once

#include <GL/freeglut.h>

#include <unordered_map>

typedef std::unordered_map<unsigned int, bool> KeyStateMap;  // 用於追蹤按鍵狀態的映射
// true -> 按下，false -> 釋放

class Point {
   public:
    Point() : x(0.0f), y(0.0f) {}
    Point(GLfloat xCoord, GLfloat yCoord)
        : x(static_cast<GLfloat>(xCoord)), y(static_cast<GLfloat>(yCoord)) {}

    void setX(GLfloat xCoord) { x = static_cast<GLfloat>(xCoord); }
    GLfloat getX() const { return x; }

    void setY(GLfloat yCoord) { y = static_cast<GLfloat>(yCoord); }
    GLfloat getY() const { return y; }

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }

    bool operator!=(const Point& other) const { return !(*this == other); }

   private:
    GLfloat x, y;
};

struct ToolEventState {
    KeyStateMap& keyStates;         // 用於追蹤按鍵狀態的映射
    KeyStateMap& specialKeyStates;  // 用於追蹤特殊按鍵狀態的映射
    Point& mousePosition;           // 當前滑鼠位置
};
