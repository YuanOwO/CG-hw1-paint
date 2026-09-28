#include "app/paint_window.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

using paint::drawing::LineCap;
using paint::drawing::LineJoin;
using paint::drawing::Tool;

namespace paint {

PaintWindow::PaintWindow(const std::string& title, int width, int height) : Window(title, width, height) {
    setupMenu();

    // 設置根元素為 CanvasElement
    auto canvas = std::make_unique<CanvasElement>(_document);
    _canvas = canvas.get();
    setRootElement(std::move(canvas));
}

void PaintWindow::setupMenu() {
    setupToolMenu();
    setupStrokeMenu();
    setupFillMenu();
    setupPointMenu();

    auto& editMenu = _menu.addSubMenu("Edit");
    editMenu.addMenuEntry("Undo", [this]() { _canvas->undo(); });
    editMenu.addMenuEntry("Redo", [this]() { _canvas->redo(); });
    editMenu.addMenuEntry("Clear", [this]() { _canvas->clear(); });

    _menu.addMenuEntry("Quit", []() { std::exit(0); });

    _menu.attach(MouseButton::MouseRight);
}

void PaintWindow::setupToolMenu() {
    auto& toolMenu = _menu.addSubMenu("Tools");

    toolMenu.addMenuEntry("Select", [this]() {});
    toolMenu.addMenuEntry("Point", [this]() { _canvas->setTool(Tool::TOOL_POINT); });
    toolMenu.addMenuEntry("Pencil", [this]() { _canvas->setTool(Tool::TOOL_PENCIL); });
    toolMenu.addMenuEntry("Line", [this]() { _canvas->setTool(Tool::TOOL_LINE); });
    toolMenu.addMenuEntry("Rectangle", [this]() { _canvas->setTool(Tool::TOOL_RECTANGLE); });
    toolMenu.addMenuEntry("Circle / Ellipse", [this]() { _canvas->setTool(Tool::TOOL_ELLIPSE); });
    toolMenu.addMenuEntry("Polygon", [this]() { _canvas->setTool(Tool::TOOL_POLYGON); });
}

void PaintWindow::setupStrokeMenu() {
    auto& strokeMenu = _menu.addSubMenu("Stroke");

    // Stroke Color Menu

    auto& colorMenu = strokeMenu.addSubMenu("Color");

    colorMenu.addMenuEntry("Black", [this]() { _canvas->getStyle().setStrokeColor(Color::Black); });
    colorMenu.addMenuEntry("White", [this]() { _canvas->getStyle().setStrokeColor(Color::White); });
    colorMenu.addMenuEntry("Red", [this]() { _canvas->getStyle().setStrokeColor(Color::Red); });
    colorMenu.addMenuEntry("Orange", [this]() { _canvas->getStyle().setStrokeColor(Color::Orange); });
    colorMenu.addMenuEntry("Yellow", [this]() { _canvas->getStyle().setStrokeColor(Color::Yellow); });
    colorMenu.addMenuEntry("Green", [this]() { _canvas->getStyle().setStrokeColor(Color::Green); });
    colorMenu.addMenuEntry("Blue", [this]() { _canvas->getStyle().setStrokeColor(Color::Blue); });
    colorMenu.addMenuEntry("Purple", [this]() { _canvas->getStyle().setStrokeColor(Color::Purple); });
    colorMenu.addMenuEntry("Custom...", [this]() {});

    // Stroke Width Menu

    auto& widthMenu = strokeMenu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", [this]() { _canvas->getStyle().setStrokeWidth(1); });
    widthMenu.addMenuEntry("3 px", [this]() { _canvas->getStyle().setStrokeWidth(3); });
    widthMenu.addMenuEntry("5 px", [this]() { _canvas->getStyle().setStrokeWidth(5); });
    widthMenu.addMenuEntry("10 px", [this]() { _canvas->getStyle().setStrokeWidth(10); });
    widthMenu.addMenuEntry("25 px", [this]() { _canvas->getStyle().setStrokeWidth(25); });
    widthMenu.addMenuEntry("50 px", [this]() { _canvas->getStyle().setStrokeWidth(50); });
    widthMenu.addMenuEntry("Thicker", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 1);
    });
    widthMenu.addMenuEntry("Thicker++", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 3);
    });
    widthMenu.addMenuEntry("Thicker+++", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 5);
    });
    widthMenu.addMenuEntry("Thinner", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 1);
    });
    widthMenu.addMenuEntry("Thinner++", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 3);
    });
    widthMenu.addMenuEntry("Thinner+++", [this]() {
        _canvas->getStyle().setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 5);
    });

    // Stroke Join Menu

    auto& joinMenu = strokeMenu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", [this]() { _canvas->getStyle().setStrokeJoin(LineJoin::MITER); });
    joinMenu.addMenuEntry("Round", [this]() { _canvas->getStyle().setStrokeJoin(LineJoin::ROUND); });
    joinMenu.addMenuEntry("Bevel", [this]() { _canvas->getStyle().setStrokeJoin(LineJoin::BEVEL); });

    // Stroke Cap Menu

    auto& capMenu = strokeMenu.addSubMenu("Cap");

    capMenu.addMenuEntry("Butt", [this]() { _canvas->getStyle().setStrokeCap(LineCap::BUTT); });
    capMenu.addMenuEntry("Round", [this]() { _canvas->getStyle().setStrokeCap(LineCap::ROUND); });
    capMenu.addMenuEntry("Square", [this]() { _canvas->getStyle().setStrokeCap(LineCap::SQUARE); });
}

void PaintWindow::setupFillMenu() {
    auto& fillMenu = _menu.addSubMenu("Fill");

    // Fill Mode Menu

    auto& fillModeMenu = fillMenu.addSubMenu("Mode");

    fillModeMenu.addMenuEntry("Outline",
                              [this]() { _canvas->getStyle().setFillMode(drawing::FillMode::OUTLINE); });
    fillModeMenu.addMenuEntry("Filled",
                              [this]() { _canvas->getStyle().setFillMode(drawing::FillMode::FILLED); });
    fillModeMenu.addMenuEntry("Advanced",
                              [this]() { _canvas->getStyle().setFillMode(drawing::FillMode::ADVANCED); });

    // Fill Color Menu

    auto& fillColorMenu = fillMenu.addSubMenu("Color");

    fillColorMenu.addMenuEntry("Transparent",
                               [this]() { _canvas->getStyle().setFillColor(Color::Transparent); });
    fillColorMenu.addMenuEntry("Black", [this]() { _canvas->getStyle().setFillColor(Color::Black); });
    fillColorMenu.addMenuEntry("White", [this]() { _canvas->getStyle().setFillColor(Color::White); });
    fillColorMenu.addMenuEntry("Red", [this]() { _canvas->getStyle().setFillColor(Color::Red); });
    fillColorMenu.addMenuEntry("Orange", [this]() { _canvas->getStyle().setFillColor(Color::Orange); });
    fillColorMenu.addMenuEntry("Yellow", [this]() { _canvas->getStyle().setFillColor(Color::Yellow); });
    fillColorMenu.addMenuEntry("Green", [this]() { _canvas->getStyle().setFillColor(Color::Green); });
    fillColorMenu.addMenuEntry("Blue", [this]() { _canvas->getStyle().setFillColor(Color::Blue); });
    fillColorMenu.addMenuEntry("Purple", [this]() { _canvas->getStyle().setFillColor(Color::Purple); });
    fillColorMenu.addMenuEntry("Custom...", [this]() {});
}

void PaintWindow::setupPointMenu() {
    auto& pointMenu = _menu.addSubMenu("Point");

    pointMenu.addMenuEntry("1 px", [this]() { _canvas->getStyle().setPointSize(1); });
    pointMenu.addMenuEntry("3 px", [this]() { _canvas->getStyle().setPointSize(3); });
    pointMenu.addMenuEntry("5 px", [this]() { _canvas->getStyle().setPointSize(5); });
    pointMenu.addMenuEntry("10 px", [this]() { _canvas->getStyle().setPointSize(10); });
    pointMenu.addMenuEntry("25 px", [this]() { _canvas->getStyle().setPointSize(25); });
    pointMenu.addMenuEntry("50 px", [this]() { _canvas->getStyle().setPointSize(50); });
}

}  // namespace paint
