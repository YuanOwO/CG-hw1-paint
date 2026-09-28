#include "app/paint_window.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

namespace paint {

PaintWindow::PaintWindow(const std::string& title, int width, int height) : Window(title, width, height) {
    setupMenu();

    // 設置根元素為 CanvasElement
    auto canvas = std::make_unique<CanvasElement>(_document);
    _canvas = canvas.get();
    setRootElement(std::move(canvas));
}

void PaintWindow::setupMenu() {
    setupShapeMenu();
    setupColorMenu();
    setupFillColorMenu();
    setupWidthMenu();
    setupJoinMenu();
    setupCapMenu();

    _menu.addMenuEntry("Clear", [this]() { _canvas->clear(); });
    _menu.addMenuEntry("Exit", []() { std::exit(0); });

    _menu.attach(MouseButton::MouseRight);
}

void PaintWindow::setupShapeMenu() {
    auto& shapeMenu = _menu.addSubMenu("Shape");

    shapeMenu.addMenuEntry("Pencil", [this]() { _canvas->setTool(drawing::Tool::TOOL_PENCIL); });
    shapeMenu.addMenuEntry("Line", [this]() { _canvas->setTool(drawing::Tool::TOOL_LINE); });
    shapeMenu.addMenuEntry("Rectangle", [this]() { _canvas->setTool(drawing::Tool::TOOL_RECTANGLE); });
    shapeMenu.addMenuEntry("Circle/Ellipse", [this]() { _canvas->setTool(drawing::Tool::TOOL_ELLIPSE); });
    shapeMenu.addMenuEntry("Polygon", [this]() { _canvas->setTool(drawing::Tool::TOOL_POLYGON); });
}

void PaintWindow::setupColorMenu() {
    auto& colorMenu = _menu.addSubMenu("Color");

    colorMenu.addMenuEntry("Black", [this]() { _canvas->setColor(Color::Black); });
    colorMenu.addMenuEntry("White", [this]() { _canvas->setColor(Color::White); });
    colorMenu.addMenuEntry("Red", [this]() { _canvas->setColor(Color::Red); });
    colorMenu.addMenuEntry("Orange", [this]() { _canvas->setColor(Color::Orange); });
    colorMenu.addMenuEntry("Yellow", [this]() { _canvas->setColor(Color::Yellow); });
    colorMenu.addMenuEntry("Green", [this]() { _canvas->setColor(Color::Green); });
    colorMenu.addMenuEntry("Blue", [this]() { _canvas->setColor(Color::Blue); });
    colorMenu.addMenuEntry("Purple", [this]() { _canvas->setColor(Color::Purple); });
}

void PaintWindow::setupFillColorMenu() {
    auto& fillColorMenu = _menu.addSubMenu("Fill Color");

    fillColorMenu.addMenuEntry("Transparent", [this]() { _canvas->setFillColor(Color::Transparent); });
    fillColorMenu.addMenuEntry("Black", [this]() { _canvas->setFillColor(Color::Black); });
    fillColorMenu.addMenuEntry("White", [this]() { _canvas->setFillColor(Color::White); });
    fillColorMenu.addMenuEntry("Red", [this]() { _canvas->setFillColor(Color::Red); });
    fillColorMenu.addMenuEntry("Orange", [this]() { _canvas->setFillColor(Color::Orange); });
    fillColorMenu.addMenuEntry("Yellow", [this]() { _canvas->setFillColor(Color::Yellow); });
    fillColorMenu.addMenuEntry("Green", [this]() { _canvas->setFillColor(Color::Green); });
    fillColorMenu.addMenuEntry("Blue", [this]() { _canvas->setFillColor(Color::Blue); });
    fillColorMenu.addMenuEntry("Purple", [this]() { _canvas->setFillColor(Color::Purple); });
}

void PaintWindow::setupWidthMenu() {
    auto& widthMenu = _menu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", [this]() { _canvas->setLineWidth(1); });
    widthMenu.addMenuEntry("3 px", [this]() { _canvas->setLineWidth(3); });
    widthMenu.addMenuEntry("5 px", [this]() { _canvas->setLineWidth(5); });
    widthMenu.addMenuEntry("10 px", [this]() { _canvas->setLineWidth(10); });
    widthMenu.addMenuEntry("25 px", [this]() { _canvas->setLineWidth(25); });
    widthMenu.addMenuEntry("50 px", [this]() { _canvas->setLineWidth(50); });
    widthMenu.addMenuEntry("Thicker", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() + 1); });
    widthMenu.addMenuEntry("Thicker++", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() + 3); });
    widthMenu.addMenuEntry("Thicker+++", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() + 5); });
    widthMenu.addMenuEntry("Thinner", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() - 1); });
    widthMenu.addMenuEntry("Thinner++", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() - 3); });
    widthMenu.addMenuEntry("Thinner+++", [this]() { _canvas->setLineWidth(_canvas->getLineWidth() - 5); });
}

void PaintWindow::setupJoinMenu() {
    auto& joinMenu = _menu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", [this]() { _canvas->setLineJoin(drawing::LineJoin::MITER); });
    joinMenu.addMenuEntry("Round", [this]() { _canvas->setLineJoin(drawing::LineJoin::ROUND); });
    joinMenu.addMenuEntry("Bevel", [this]() { _canvas->setLineJoin(drawing::LineJoin::BEVEL); });
}

void PaintWindow::setupCapMenu() {
    auto& capMenu = _menu.addSubMenu("Cap");

    capMenu.addMenuEntry("Butt", [this]() { _canvas->setLineCap(drawing::LineCap::BUTT); });
    capMenu.addMenuEntry("Round", [this]() { _canvas->setLineCap(drawing::LineCap::ROUND); });
    capMenu.addMenuEntry("Square", [this]() { _canvas->setLineCap(drawing::LineCap::SQUARE); });
}

}  // namespace paint
