#include "menu.hpp"

#include <GL/freeglut.h>

#include <algorithm>

#include "canvas.hpp"
#include "color.hpp"
#include "confirm.hpp"
#include "shapeStyle.hpp"

namespace menu {
namespace {

int mainMenuId = 0;
int menuWindowId = 0;

void setMenuEnabled(bool enabled) {
    if (!mainMenuId || !menuWindowId) return;

    int previousWindow = glutGetWindow();
    int previousMenu = glutGetMenu();

    glutSetWindow(menuWindowId);

    if (enabled) {
        glutSetMenu(mainMenuId);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    } else {
        glutDetachMenu(GLUT_RIGHT_BUTTON);
    }

    // 回到原本的視窗和選單，避免影響其他程式邏輯。
    if (previousMenu) glutSetMenu(previousMenu);
    if (previousWindow) glutSetWindow(previousWindow);
}

#pragma region Shape Menu

void shapeMenu(int option) {
    auto tool = static_cast<canvas::Tool>(option);

    canvas::setTool(tool);
}

int addShapeMenu() {
    auto menu = glutCreateMenu(shapeMenu);

    glutAddMenuEntry("Pencil", (int)canvas::Tool::TOOL_PENCIL);
    glutAddMenuEntry("Line", (int)canvas::Tool::TOOL_LINE);
    glutAddMenuEntry("Rectangle", (int)canvas::Tool::TOOL_RECTANGLE);
    glutAddMenuEntry("Circle/Ellipse", (int)canvas::Tool::TOOL_ELLIPSE);
    glutAddMenuEntry("Polygon", (int)canvas::Tool::TOOL_POLYGON);

    return menu;
}

#pragma endregion  // Shape Menu

#pragma region Color Menu

void colorMenu(int option) {
    auto color = static_cast<color::Color>(option);

    canvas::setColor(color::ColorRGBA(color));
}

int addColorMenu() {
    auto menu = glutCreateMenu(colorMenu);

    glutAddMenuEntry("Black", (int)color::Color::Black);
    glutAddMenuEntry("White", (int)color::Color::White);
    glutAddMenuEntry("Red", (int)color::Color::Red);
    glutAddMenuEntry("Orange", (int)color::Color::Orange);
    glutAddMenuEntry("Yellow", (int)color::Color::Yellow);
    glutAddMenuEntry("Green", (int)color::Color::Green);
    glutAddMenuEntry("Blue", (int)color::Color::Blue);
    glutAddMenuEntry("Purple", (int)color::Color::Purple);

    return menu;
}

#pragma endregion  // Color Menu

#pragma region Fill Color Menu

void fillColorMenu(int option) {
    auto color = static_cast<color::Color>(option);

    canvas::setFillColor(color::ColorRGBA(color));
}

int addFillColorMenu() {
    auto menu = glutCreateMenu(fillColorMenu);

    glutAddMenuEntry("Transparent", (int)color::Color::Transparent);
    glutAddMenuEntry("Black", (int)color::Color::Black);
    glutAddMenuEntry("White", (int)color::Color::White);
    glutAddMenuEntry("Red", (int)color::Color::Red);
    glutAddMenuEntry("Orange", (int)color::Color::Orange);
    glutAddMenuEntry("Yellow", (int)color::Color::Yellow);
    glutAddMenuEntry("Green", (int)color::Color::Green);
    glutAddMenuEntry("Blue", (int)color::Color::Blue);
    glutAddMenuEntry("Purple", (int)color::Color::Purple);

    return menu;
}

#pragma endregion  // Fill Color Menu

#pragma region Width Menu

void widthMenu(int option) {
    int width = canvas::getLineWidth();

    switch (static_cast<Menu_Width>(option)) {
    case Menu_Width::WIDTH_THICKER:
        width += 1;
        break;
    case Menu_Width::WIDTH_THICKERRR:
        width += 3;
        break;
    case Menu_Width::WIDTH_THICKERRRRR:
        width += 5;
        break;
    case Menu_Width::WIDTH_THINNER:
        width -= 1;
        break;
    case Menu_Width::WIDTH_THINNERRR:
        width -= 3;
        break;
    case Menu_Width::WIDTH_THINNERRRRR:
        width -= 5;
        break;
    default:
        width = option;
        break;
    }

    width = std::max(1, width);  // Ensure width is at least 1

    canvas::setLineWidth(width);
}

int addWidthMenu() {
    auto menu = glutCreateMenu(widthMenu);

    glutAddMenuEntry("1 px", (int)Menu_Width::WIDTH_1);
    glutAddMenuEntry("3 px", (int)Menu_Width::WIDTH_3);
    glutAddMenuEntry("5 px", (int)Menu_Width::WIDTH_5);
    glutAddMenuEntry("10 px", (int)Menu_Width::WIDTH_10);
    glutAddMenuEntry("25 px", (int)Menu_Width::WIDTH_25);
    glutAddMenuEntry("50 px", (int)Menu_Width::WIDTH_50);
    glutAddMenuEntry("Thicker", (int)Menu_Width::WIDTH_THICKER);
    glutAddMenuEntry("Thicker++", (int)Menu_Width::WIDTH_THICKERRR);
    glutAddMenuEntry("Thicker+++", (int)Menu_Width::WIDTH_THICKERRRRR);
    glutAddMenuEntry("Thinner", (int)Menu_Width::WIDTH_THINNER);
    glutAddMenuEntry("Thinner++", (int)Menu_Width::WIDTH_THINNERRR);
    glutAddMenuEntry("Thinner+++", (int)Menu_Width::WIDTH_THINNERRRRR);

    return menu;
}

#pragma endregion  // Width Menu

#pragma region Join Menu

void lineJoinMenu(int option) {
    auto join = static_cast<shape::LineJoin>(option);

    canvas::setLineJoin(join);
}

int addLineJoinMenu() {
    auto menu = glutCreateMenu(lineJoinMenu);

    glutAddMenuEntry("Miter", (int)shape::LineJoin::MITER);
    glutAddMenuEntry("Bevel", (int)shape::LineJoin::BEVEL);
    glutAddMenuEntry("Round", (int)shape::LineJoin::ROUND);

    return menu;
}

#pragma endregion  // Join Menu

#pragma region Main Menu

void requestClear() {
    setMenuEnabled(false);
    confirm::showConfirmationWindow(
        "OpenGL Painter - Confirm Clear", "Clear canvas and all undo/redo history?\nThis cannot be undone.",
        []() {
            setMenuEnabled(true);
            canvas::clearCanvas();
        },
        []() { setMenuEnabled(true); });
}

void requestQuit() {
    setMenuEnabled(false);
    confirm::showConfirmationWindow(
        "OpenGL Painter - Confirm Quit", "Are you sure you want to quit?\nAny unsaved work will be lost.",
        []() { exit(0); }, []() { setMenuEnabled(true); });
}

void mainMenu(int option) {
    if (option == (int)Menu_Main::MENU_CLEAR) {
        requestClear();
    } else if (option == (int)Menu_Main::MENU_QUIT) {
        requestQuit();
    }
}

int addMainMenu() {
    auto menu_shape = addShapeMenu();
    auto menu_color = addColorMenu();
    auto menu_fill_color = addFillColorMenu();
    auto menu_width = addWidthMenu();
    auto menu_join = addLineJoinMenu();

    auto menu = glutCreateMenu(mainMenu);
    glutAddSubMenu("Shape", menu_shape);
    glutAddSubMenu("Color", menu_color);
    glutAddSubMenu("Fill Color", menu_fill_color);
    glutAddSubMenu("Line Width", menu_width);
    glutAddSubMenu("Line Join Mode", menu_join);
    glutAddMenuEntry("Clear", (int)Menu_Main::MENU_CLEAR);
    glutAddMenuEntry("Quit", (int)Menu_Main::MENU_QUIT);

    return menu;
}

#pragma endregion  // Main Menu

}  // namespace

void init() {
    menuWindowId = glutGetWindow();
    mainMenuId = addMainMenu();
    setMenuEnabled(true);
}

}  // namespace menu
