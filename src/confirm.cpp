#include "confirm.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <string>
#include <vector>

namespace confirm {
namespace {

int parentWindow = 0;
int dialogWindow = 0;

enum class MoustPressedButton {
    None = -1,
    Cancel,
    Confirm,
};

////////////////////////////////////////////////////////////////////////

const int CONFIRM_WIDTH = 480;
const int CONFIRM_HEIGHT = 240;

const int BUTTON_WIDTH = 120;
const int BUTTON_HEIGHT = 30;
const int BUTTON_MARGIN = 20;

const int BUTTON_TOP = CONFIRM_HEIGHT - BUTTON_MARGIN - BUTTON_HEIGHT;
const int BUTTON_BOTTOM = BUTTON_TOP + BUTTON_HEIGHT;

const int BUTTON_LEFT[] = {
    CONFIRM_WIDTH - 2 * (BUTTON_MARGIN + BUTTON_WIDTH),  // Cancel button
    CONFIRM_WIDTH - BUTTON_MARGIN - BUTTON_WIDTH,        // Confirm button
};

const int BUTTON_RIGHT[] = {
    BUTTON_LEFT[0] + BUTTON_WIDTH,  // Cancel button
    BUTTON_LEFT[1] + BUTTON_WIDTH,  // Confirm button
};

const char* BUTTON_LABELS[] = {
    "Cancel",
    "Confirm",
};

const int MAX_MESSAGE_LINES = 7;

bool isPressed[] = {false, false};

MoustPressedButton pressedButton(int x, int y) {
    if (y < BUTTON_TOP || y >= BUTTON_BOTTOM) return MoustPressedButton::None;

    for (auto btn : {MoustPressedButton::Cancel, MoustPressedButton::Confirm}) {
        int index = static_cast<int>(btn);
        if (x >= BUTTON_LEFT[index] && x < BUTTON_RIGHT[index]) {
            return btn;
        }
    }

    return MoustPressedButton::None;
}

////////////////////////////////////////////////////////////////////////

std::vector<std::string> lines;

void (*confirmCallback)() = nullptr;
void (*cancelCallback)() = nullptr;

////////////////////////////////////////////////////////////////////////

int getTextWidth(const std::string& text) {
    return glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(text.c_str()));
}

void drawText(int x, int y, const std::string& text) {
    glRasterPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(text.c_str()));
}

////////////////////////////////////////////////////////////////////////

// 按像素寬度換行，並保留呼叫端提供的換行符號。
void wrapMessage(const char* message) {
    lines.clear();
    std::string line;
    int width = 0;
    for (unsigned char c : std::string(message ? message : "")) {
        int characterWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
        if (c == '\n' || width + characterWidth > CONFIRM_WIDTH - 40) {
            lines.push_back(line);
            line.clear();
            width = 0;
            if (c == '\n') continue;
        }
        line += static_cast<char>(c);
        width += characterWidth;
    }
    lines.push_back(line);
    if (lines.size() > MAX_MESSAGE_LINES) {
        lines.resize(MAX_MESSAGE_LINES);
        auto& last = lines.back();
        while (!last.empty() && getTextWidth(last + "...") > CONFIRM_WIDTH - 40) {
            last.pop_back();
        }
        last += "...";
    }
}

////////////////////////////////////////////////////////////////////////

void display() {
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.1f, 0.1f, 0.1f);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        drawText(20, 35 + static_cast<int>(i) * 24, lines[i]);
    }

    for (auto btn : {MoustPressedButton::Cancel, MoustPressedButton::Confirm}) {
        int idx = static_cast<int>(btn);
        auto gray = isPressed[idx] ? 0.65f : 0.82f;

        glColor3f(gray, gray, gray);

        glRecti(BUTTON_LEFT[idx], BUTTON_TOP, BUTTON_RIGHT[idx], BUTTON_BOTTOM);

        glColor3f(0.1f, 0.1f, 0.1f);
        int textWidth = getTextWidth(BUTTON_LABELS[idx]);
        drawText(BUTTON_LEFT[idx] + (BUTTON_WIDTH - textWidth) / 2, BUTTON_TOP + 24, BUTTON_LABELS[idx]);
    }

    glFlush();
}

void reshape(int width, int height) {
    // 保持確認視窗大小不變
    if (width != CONFIRM_WIDTH || height != CONFIRM_HEIGHT) {
        glutReshapeWindow(CONFIRM_WIDTH, CONFIRM_HEIGHT);
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, CONFIRM_WIDTH, CONFIRM_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutPostRedisplay();
}

void finish(MoustPressedButton confirmed) {
    if (!dialogWindow) return;

    int window = dialogWindow;
    int parent = parentWindow;
    auto callback = confirmed == MoustPressedButton::Confirm ? confirmCallback : cancelCallback;

    // 重置狀態
    dialogWindow = 0;
    parentWindow = 0;
    confirmCallback = cancelCallback = nullptr;
    lines.clear();

    // 主動關閉時移除 close callback，避免稍後再次觸發取消。
    glutSetWindow(window);
    glutCloseFunc(nullptr);
    glutDestroyWindow(window);

    if (parent) {
        glutSetWindow(parent);
        glutPostRedisplay();
    }

    // 回呼可能重開確認視窗，因此先完成上述清理。
    if (callback) callback();
}

void keyboard(unsigned char key, int, int) {
    if (key == '\r')  // Enter
        finish(MoustPressedButton::Confirm);
    else if (key == 27)  // ESC
        finish(MoustPressedButton::Cancel);
}

void mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON) return;

    auto hit = pressedButton(x, y);

    if (hit == MoustPressedButton::None) return;
    auto idx = static_cast<int>(hit);

    if (state == GLUT_DOWN) {
        isPressed[idx] = true;
    } else {  // state == GLUT_UP
        isPressed[idx] = false;
        finish(hit);
    }
    glutPostRedisplay();
}

}  // namespace

bool isOpen() {
    return dialogWindow != 0;
}

void showConfirmationWindow(const char* windowTitle, const char* message, void (*onConfirm)(),
                            void (*onCancel)()) {
    int prevWindow = glutGetWindow();

    // 若確認視窗已開啟，則將其顯示並置頂，並更新回呼函式。
    if (dialogWindow) {
        glutSetWindow(dialogWindow);
        glutShowWindow();
        glutPopWindow();
        if (prevWindow) glutSetWindow(prevWindow);
        return;
    }

    parentWindow = prevWindow;

    // 將確認視窗置中於父視窗，若沒有父視窗則使用預設位置。
    int x = 100, y = 100;
    if (parentWindow) {
        x = glutGet(GLUT_WINDOW_X) + (glutGet(GLUT_WINDOW_WIDTH) - CONFIRM_WIDTH) / 2;
        y = glutGet(GLUT_WINDOW_Y) + (glutGet(GLUT_WINDOW_HEIGHT) - CONFIRM_HEIGHT) / 2;
    }

    // 註冊 callback
    confirmCallback = onConfirm;
    cancelCallback = onCancel;
    wrapMessage(message);

    int previousDisplayMode = glutGet(GLUT_INIT_DISPLAY_MODE);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(CONFIRM_WIDTH, CONFIRM_HEIGHT);
    glutInitWindowPosition(std::max(0, x), std::max(0, y));
    glutInitDisplayMode(previousDisplayMode);
    dialogWindow = glutCreateWindow(windowTitle ? windowTitle : "Confirm");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutIgnoreKeyRepeat(1);
    reshape(CONFIRM_WIDTH, CONFIRM_HEIGHT);
}

}  // namespace confirm
