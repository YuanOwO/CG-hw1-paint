#include "app/paintWindow.hpp"

namespace paint {

PaintWindow::PaintWindow(const std::string& title, int width, int height) : Window(title, width, height) {
    setupMenu();
}

void PaintWindow::setupMenu() {
    setupShapeMenu();
    setupColorMenu();
    setupFillColorMenu();
    setupWidthMenu();
    setupJoinMenu();
    setupCapMenu();

    _menu.addMenuEntry("Clear", []() {});
    _menu.addMenuEntry("Exit", []() { exit(0); });

    _menu.attach(MouseButton::MouseRight);
}

void PaintWindow::setupShapeMenu() {
    auto& shapeMenu = _menu.addSubMenu("Shape");

    shapeMenu.addMenuEntry("Pencil", []() {});
    shapeMenu.addMenuEntry("Line", []() {});
    shapeMenu.addMenuEntry("Rectangle", []() {});
    shapeMenu.addMenuEntry("Circle/Ellipse", []() {});
    shapeMenu.addMenuEntry("Polygon", []() {});
}

void PaintWindow::setupColorMenu() {
    auto& colorMenu = _menu.addSubMenu("Color");

    colorMenu.addMenuEntry("Black", []() {});
    colorMenu.addMenuEntry("White", []() {});
    colorMenu.addMenuEntry("Red", []() {});
    colorMenu.addMenuEntry("Orange", []() {});
    colorMenu.addMenuEntry("Yellow", []() {});
    colorMenu.addMenuEntry("Green", []() {});
    colorMenu.addMenuEntry("Blue", []() {});
    colorMenu.addMenuEntry("Purple", []() {});
}

void PaintWindow::setupFillColorMenu() {
    auto& fillColorMenu = _menu.addSubMenu("Fill Color");

    fillColorMenu.addMenuEntry("Transparent", []() {});
    fillColorMenu.addMenuEntry("Black", []() {});
    fillColorMenu.addMenuEntry("White", []() {});
    fillColorMenu.addMenuEntry("Red", []() {});
    fillColorMenu.addMenuEntry("Orange", []() {});
    fillColorMenu.addMenuEntry("Yellow", []() {});
    fillColorMenu.addMenuEntry("Green", []() {});
    fillColorMenu.addMenuEntry("Blue", []() {});
    fillColorMenu.addMenuEntry("Purple", []() {});
}

void PaintWindow::setupWidthMenu() {
    auto& widthMenu = _menu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", []() {});
    widthMenu.addMenuEntry("3 px", []() {});
    widthMenu.addMenuEntry("5 px", []() {});
    widthMenu.addMenuEntry("10 px", []() {});
    widthMenu.addMenuEntry("25 px", []() {});
    widthMenu.addMenuEntry("50 px", []() {});
    widthMenu.addMenuEntry("Thicker", []() {});
    widthMenu.addMenuEntry("Thicker++", []() {});
    widthMenu.addMenuEntry("Thicker+++", []() {});
    widthMenu.addMenuEntry("Thinner", []() {});
    widthMenu.addMenuEntry("Thinner++", []() {});
    widthMenu.addMenuEntry("Thinner+++", []() {});
}

void PaintWindow::setupJoinMenu() {
    auto& joinMenu = _menu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", []() {});
    joinMenu.addMenuEntry("Round", []() {});
    joinMenu.addMenuEntry("Bevel", []() {});
}

void PaintWindow::setupCapMenu() {
    auto& capMenu = _menu.addSubMenu("Cap");

    capMenu.addMenuEntry("Butt", []() {});
    capMenu.addMenuEntry("Round", []() {});
    capMenu.addMenuEntry("Square", []() {});
}

}  // namespace paint
