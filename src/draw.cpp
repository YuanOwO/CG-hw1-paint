#include "draw.hpp"

#include <GL/freeglut.h>

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "color.hpp"
#include "confirm.hpp"
#include "shape.hpp"
#include "types.hpp"

namespace draw {

namespace {

typedef std::unique_ptr<shape::Shape> ShapePtr;

// true -> 按下，false -> 釋放
KeyStateMap keyStates, specialKeyStates;

Tool currentTool = Tool::TOOL_PENCIL;
int lineWidth = 1;
color::ColorRGBA currentColor = color::ColorRGBA(color::Color::Black);
color::ColorRGBA currentFillColor = color::ColorRGBA(color::Color::Transparent);

ShapePtr draft = nullptr;
std::vector<ShapePtr> history;
std::vector<ShapePtr> redoStack;

bool isDrawing() {
    return draft != nullptr;
}

void clearDraft() {
    draft.reset();
}

////////////////////////////////////////////////////////////////////////

void undo() {
    if (isDrawing()) {  // 取消草稿
        clearDraft();
    }

    if (history.empty()) {  // 沒東西可以 undo
        return;
    }

    // 將最後一個歷史紀錄移到 redo 堆疊中
    redoStack.push_back(std::move(history.back()));
    history.pop_back();
    glutPostRedisplay();
}

void redo() {
    if (isDrawing()) {  // 畫畫時不可以 redo
        return;
    }

    if (redoStack.empty()) {  // 沒東西可以 redo
        return;
    }

    // 將最後一個 redo 堆疊移回歷史紀錄中
    history.push_back(std::move(redoStack.back()));
    redoStack.pop_back();
    glutPostRedisplay();
}

void onAnyKeyDown() {
    bool z = keyStates['z'] || keyStates['Z'];
    bool y = keyStates['y'] || keyStates['Y'];
    bool shift = specialKeyStates[GLUT_KEY_SHIFT_L];
    bool ctrl = specialKeyStates[GLUT_KEY_CTRL_L];
    bool cmd = specialKeyStates[GLUT_KEY_SUPER_L];

#ifdef __APPLE__
    if (cmd && z) {
        if (shift) {
            redo();
        } else {
            undo();
        }
    }
#else
    if (ctrl && z) {
        if (shift) {
            redo();
        } else {
            undo();
        }
    } else if (ctrl && y && !shift) {
        redo();
    }
#endif
}

////////////////////////////////////////////////////////////////////////

void createDraft() {
    GLfloat width = static_cast<GLfloat>(lineWidth);
    GLfloat color[4], fillColor[4];

    for (int i = 0; i < 4; ++i) {
        color[i] = static_cast<GLfloat>(currentColor[i]);
        fillColor[i] = static_cast<GLfloat>(currentFillColor[i]);
    }

    if (isDrawing()) {
        clearDraft();
    }

    switch (currentTool) {
    case Tool::TOOL_PENCIL:
        draft = std::make_unique<shape::Stroke>(width, color, fillColor);
        break;
    case Tool::TOOL_LINE:
        draft = std::make_unique<shape::Line>(width, color, fillColor);
        break;
    case Tool::TOOL_RECTANGLE:
        draft = std::make_unique<shape::Rectangle>(width, color, fillColor);
        break;
    default:
        draft = nullptr;
        break;
    }
}

void commitDraft() {
    if (!isDrawing()) {
        return;
    }

    history.push_back(std::move(draft));
    redoStack.clear();  // 清空重做堆疊，因為新的操作會使之前的重做無效
    clearDraft();
    glutPostRedisplay();
}

}  // namespace

////////////////////////////////////////////////////////////////////////

void setTool(Tool tool) {
    currentTool = tool;
}

Tool getTool() {
    return currentTool;
}

void setWidth(int width) {
    lineWidth = width;
}

int getWidth() {
    return lineWidth;
}

void setColor(const color::ColorRGBA& color) {
    currentColor = color;
}

color::ColorRGBA getColor() {
    return currentColor;
}

void setFillColor(const color::ColorRGBA& color) {
    currentFillColor = color;
}

color::ColorRGBA getFillColor() {
    return currentFillColor;
}

////////////////////////////////////////////////////////////////////////

void clear() {
    history.clear();
    redoStack.clear();
    clearDraft();
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////

void init() {}

////////////////////////////////////////////////////////////////////////

void mouse(int button, int state, int x, int y) {
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (!isDrawing()) {
                createDraft();
            }

            if (isDrawing() && draft->onMouseDown(eventState)) {
                commitDraft();
            }
        } else if (state == GLUT_UP) {
            if (isDrawing() && draft->onMouseUp(eventState)) {
                commitDraft();
            }
        }
    }
}

void motion(int x, int y) {
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing() && draft->onMouseMove(eventState)) {
        commitDraft();
    }
}

void keyDown(unsigned char key, int x, int y) {
    keyStates[key] = true;
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing() && draft->onMouseMove(eventState)) {
        commitDraft();
    }

    onAnyKeyDown();
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing() && draft->onMouseMove(eventState)) {
        commitDraft();
    }
}

void specialKeyDown(int key, int x, int y) {
    specialKeyStates[key] = true;
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing() && draft->onMouseMove(eventState)) {
        commitDraft();
    }

    onAnyKeyDown();
}

void specialKeyUp(int key, int x, int y) {
    specialKeyStates[key] = false;
    if (confirm::isOpen()) return;
    auto point = Point(x, y);
    EventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing() && draft->onMouseMove(eventState)) {
        commitDraft();
    }
}

////////////////////////////////////////////////////////////////////////

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& shape : history) {
        shape->draw();
    }

    if (draft) {
        draft->draw();
    }

    glFlush();
}

void reshape(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, static_cast<GLdouble>(width), static_cast<GLdouble>(height), 0.0, -1.0, 1.0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutPostRedisplay();
}

}  // namespace draw
