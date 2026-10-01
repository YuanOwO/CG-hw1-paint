#include "app/windows/paint_window.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "app/application.hpp"
#include "ui/layout/bounding.hpp"

using paint::drawing::LineCap;
using paint::drawing::LineJoin;
using paint::drawing::Tool;

namespace paint::app {

PaintWindow::PaintWindow(Application& app, const std::string& title, int width, int height)
    : Window(app, title, width, height) {
    setupMenu();

    BoundingBox canvasBounds(0, 0, width, height);  // 預留 10 像素給菜單

    // 設置根元素為 CanvasElement
    auto canvas = std::make_unique<ui::CanvasElement>(canvasBounds, _document);
    _canvas = canvas.get();
    setRootElement(std::move(canvas));
    setFocusedElement(_canvas);  // 將焦點設置為 CanvasElement

    // 設置快捷鍵
    _shortcutManager.bind({Mod::Primary, Key::Z}, [this]() { _canvas->undo(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::Z}, [this]() { _canvas->redo(); });

    _shortcutManager.bind({Mod::Primary, Key::S}, [this]() { _document.save(); });
    _shortcutManager.bind({Mod::Primary, Key::N}, [this]() { newFile(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::N}, [this]() { newWindow(); });
    _shortcutManager.bind({Mod::Primary, Key::R}, [this]() {
        requestCachedRedisplay();
        resetInputState();
    });
    _shortcutManager.bind({Mod::Ctrl, Key::C}, [this]() { std::exit(0); });
    _shortcutManager.bind({Key::F5}, [this]() { _canvas->clear(); });

    // 設置工具快捷鍵
    _shortcutManager.bind({Key::Digit1}, [this]() { _canvas->setTool(Tool::TOOL_PENCIL); });
    _shortcutManager.bind({Key::Digit2}, [this]() { _canvas->setTool(Tool::TOOL_LINE); });
    _shortcutManager.bind({Key::Digit3}, [this]() { _canvas->setTool(Tool::TOOL_RECTANGLE); });
    _shortcutManager.bind({Key::Digit4}, [this]() { _canvas->setTool(Tool::TOOL_ELLIPSE); });
    _shortcutManager.bind({Key::Digit5}, [this]() { _canvas->setTool(Tool::TOOL_POLYGON); });

    _shortcutManager.bind({Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(std::max(1, static_cast<int>(style.stroke.width - 1)));
        _canvas->setStyle(style);
    });
    _shortcutManager.bind({Key::RightBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(style.stroke.width + 1);
        _canvas->setStyle(style);
    });
    _shortcutManager.bind({Mod::Shift, Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Black);
        _canvas->setStyle(style);
    });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (_shortcutManager.handle(event)) {
            event.stopPropagation();
            return;
        }
    });
}

void PaintWindow::newFile() {
    _document.newFile();
    requestRedisplay();
}

void PaintWindow::newWindow() {
    app().createWindow<PaintWindow>(app().name(), width(), height());
}

#pragma region Main Menu

void PaintWindow::setupMenu() {
    auto& fileMenu = _menu.addSubMenu("File");
    fileMenu.addMenuEntry("New", [this]() { newFile(); });
    fileMenu.addMenuEntry("New Window", [this]() { newWindow(); });
    fileMenu.addMenuEntry("Load", [this]() {});
    fileMenu.addMenuEntry("Save", [this]() { _document.save(); });
    fileMenu.addMenuEntry("Save As", [this]() {});
    fileMenu.addMenuEntry("Export", [this]() {});

    auto& editMenu = _menu.addSubMenu("Edit");
    editMenu.addMenuEntry("Undo", [this]() { _canvas->undo(); });
    editMenu.addMenuEntry("Redo", [this]() { _canvas->redo(); });
    editMenu.addMenuEntry("Clear", [this]() { _canvas->clear(); });

    setupToolMenu();
    setupStrokeMenu();
    setupFillMenu();
    setupPointMenu();

    _menu.addMenuEntry("Grid", [this]() { _canvas->setGridVisibility(!_canvas->isGridVisible()); });

    _menu.addMenuEntry("Close", [this]() { this->close(); });

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
        auto style = _canvas->style();
        style.setStrokeColor(Color::Black);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::White);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Red);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Orange);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Yellow);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Green);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Blue);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Purple);
        _canvas->setStyle(style);
    });
    colorMenu.addMenuEntry("Custom...", [this]() {});

    // Stroke Width Menu

    auto& widthMenu = strokeMenu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(5);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(10);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(25);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(50);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thicker+++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 5);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 1);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 3);
        _canvas->setStyle(style);
    });
    widthMenu.addMenuEntry("Thinner+++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 5);
        _canvas->setStyle(style);
    });

    // Stroke Join Menu

    auto& joinMenu = strokeMenu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::MITER);
        _canvas->setStyle(style);
    });
    joinMenu.addMenuEntry("Bevel", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::BEVEL);
        _canvas->setStyle(style);
    });
    joinMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::ROUND);
        _canvas->setStyle(style);
    });

    // Stroke Cap Menu

    auto& capMenu = strokeMenu.addSubMenu("Cap");

    capMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->style();
        style.setStrokeCap(LineCap::ROUND);
        _canvas->setStyle(style);
    });
    capMenu.addMenuEntry("Square", [this]() {
        auto style = _canvas->style();
        style.setStrokeCap(LineCap::SQUARE);
        _canvas->setStyle(style);
    });
    capMenu.addMenuEntry("Butt", [this]() {
        auto style = _canvas->style();
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
        auto style = _canvas->style();
        style.setFillMode(drawing::FillMode::OUTLINE);
        _canvas->setStyle(style);
    });
    fillModeMenu.addMenuEntry("Filled", [this]() {
        auto style = _canvas->style();
        style.setFillMode(drawing::FillMode::FILLED);
        _canvas->setStyle(style);
    });
    fillModeMenu.addMenuEntry("Advanced", [this]() {
        auto style = _canvas->style();
        style.setFillMode(drawing::FillMode::ADVANCED);
        _canvas->setStyle(style);
    });

    // Fill Color Menu

    auto& fillColorMenu = fillMenu.addSubMenu("Color");

    fillColorMenu.addMenuEntry("Transparent", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Transparent);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Black", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Black);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::White);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Red);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Orange);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Yellow);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Green);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Blue);
        _canvas->setStyle(style);
    });
    fillColorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->style();
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
        auto style = _canvas->style();
        style.setPointSize(1);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(3);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(5);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(10);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(25);
        _canvas->setStyle(style);
    });
    pointMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(50);
        _canvas->setStyle(style);
    });
}

#pragma endregion  // Point Menu

}  // namespace paint::app
