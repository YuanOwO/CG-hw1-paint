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

#pragma region Main Menu

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

#pragma endregion  // Main Menu

#pragma region Tool Menu

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

#pragma endregion  // Tool Menu

#pragma region Stroke Menu

void PaintWindow::setupStrokeMenu() {
    auto& strokeMenu = _menu.addSubMenu("Stroke");

    // Stroke Color Menu

    auto& colorMenu = strokeMenu.addSubMenu("Color");

    colorMenu.addMenuEntry("Black", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Black);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::White);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Red);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Orange);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Yellow);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Green);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Blue);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeColor(Color::Purple);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Custom...", [this]() {});

    // Stroke Width Menu

    auto& widthMenu = strokeMenu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(5);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(10);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(25);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(50);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker++", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker+++", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() + 5);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner++", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner+++", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeWidth(_canvas->getStyle().getStrokeWidth() - 5);
        _canvas->setStyle(style);
    });

    // Stroke Join Menu

    auto& joinMenu = strokeMenu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeJoin(LineJoin::MITER);
        _canvas->setStyle(style);
    });
    joinMenu.addMenuEntry("Bevel", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeJoin(LineJoin::BEVEL);
        _canvas->setStyle(style);
    });
    joinMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeJoin(LineJoin::ROUND);
        _canvas->setStyle(style);
    });

    // Stroke Cap Menu

    auto& capMenu = strokeMenu.addSubMenu("Cap");

    capMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeCap(LineCap::ROUND);
        _canvas->setStyle(style);
    });
    capMenu.addMenuEntry("Square", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeCap(LineCap::SQUARE);
        _canvas->setStyle(style);
    });
    capMenu.addMenuEntry("Butt", [this]() {
        auto style = _canvas->getStyle();
        style.setStrokeCap(LineCap::BUTT);
        _canvas->setStyle(style);
    });
}

#pragma endregion  // Stroke Menu

#pragma region Fill Menu

void PaintWindow::setupFillMenu() {
    auto& fillMenu = _menu.addSubMenu("Fill");

    // Fill Mode Menu

    auto& fillModeMenu = fillMenu.addSubMenu("Mode");

    fillModeMenu.addMenuEntry("Outline", [this]() {
        auto style = _canvas->getStyle();
        style.setFillMode(drawing::FillMode::OUTLINE);
        _canvas->setStyle(style);
    });
    fillModeMenu.addMenuEntry("Filled", [this]() {
        auto style = _canvas->getStyle();
        style.setFillMode(drawing::FillMode::FILLED);
        _canvas->setStyle(style);
    });
    fillModeMenu.addMenuEntry("Advanced", [this]() {
        auto style = _canvas->getStyle();
        style.setFillMode(drawing::FillMode::ADVANCED);
        _canvas->setStyle(style);
    });

    // Fill Color Menu

    auto& fillColorMenu = fillMenu.addSubMenu("Color");

    fillColorMenu.addMenuEntry("Transparent", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Transparent);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Black", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Black);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::White);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Red);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Orange);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Yellow);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Green);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Blue);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->getStyle();
        style.setFillColor(Color::Purple);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Custom...", [this]() {});
}

#pragma endregion  // Fill Menu

#pragma region Point Menu

void PaintWindow::setupPointMenu() {
    auto& pointMenu = _menu.addSubMenu("Point");

    pointMenu.addMenuEntry("1 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(1);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(3);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(5);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(10);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(25);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->getStyle();
        style.setPointSize(50);
        _canvas->setStyle(style);
    });
}

#pragma endregion  // Point Menu

}  // namespace paint
