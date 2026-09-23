#include "draw.hpp"

#include <GL/freeglut.h>

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "color.hpp"
#include "confirm.hpp"
#include "drawing_tool.hpp"
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

// 目前正在使用的繪圖工具，若為 nullptr 則表示沒有正在繪製的草稿。
std::unique_ptr<drawing::IDrawingTool> activeTool;

std::vector<ShapePtr> history;
std::vector<ShapePtr> redoStack;

////////////////////////////////////////////////////////////////////////

bool isDrawing() {
    return activeTool != nullptr;
}

void clearDraft() {
    activeTool.reset();
}

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

    activeTool = drawing::createDrawingTool(currentTool, width, color, fillColor);
}

////////////////////////////////////////////////////////////////////////

void undo() {
    if (isDrawing()) {  // 取消草稿
        clearDraft();
        glutPostRedisplay();
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
#else  // Windows/Linux
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

void handleDraftEvent(drawing::ToolEventResult result) {
    switch (result) {
    case drawing::ToolEventResult::COMMIT:
        history.push_back(activeTool->takeShape());
        redoStack.clear();  // 清空 redo 堆疊，因為新的操作會使 redo 無效
        [[fallthrough]];
    case drawing::ToolEventResult::CANCEL:  // 注意：這裡故意不加 break，讓 COMMIT 也會清除草稿
        clearDraft();
        break;
    default:
        // 不需要提交草稿，繼續繪製
        break;
    }

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

void clearCanvas() {
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
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (!isDrawing()) {
                createDraft();
            }

            if (isDrawing()) {
                handleDraftEvent(activeTool->onMouseDown(eventState));
            }
        } else if (state == GLUT_UP) {
            if (isDrawing()) {
                handleDraftEvent(activeTool->onMouseUp(eventState));
            }
        }
    }
}

void motion(int x, int y) {
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onMouseMove(eventState));
    }
}

void passiveMotion(int x, int y) {
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onMousePassiveMove(eventState));
    }
}

void keyDown(unsigned char key, int x, int y) {
    keyStates[key] = true;
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onKeyDown(eventState));
    }

    onAnyKeyDown();
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onKeyUp(eventState));
    }
}

void specialKeyDown(int key, int x, int y) {
    specialKeyStates[key] = true;
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onSpecialKeyDown(eventState));
    }

    onAnyKeyDown();
}

void specialKeyUp(int key, int x, int y) {
    specialKeyStates[key] = false;
    if (confirm::isOpen()) return;
    auto point = Point(x, y);
    ToolEventState eventState{keyStates, specialKeyStates, point};

    if (isDrawing()) {
        handleDraftEvent(activeTool->onSpecialKeyUp(eventState));
    }
}

////////////////////////////////////////////////////////////////////////

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& shape : history) {
        shape->draw();
    }

    // 繪製草稿
    if (activeTool) {
        activeTool->preview().draw();
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
