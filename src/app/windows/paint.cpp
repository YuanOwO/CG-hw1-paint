#include "app/windows/paint.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "app/application.hpp"
#include "common/font.hpp"
#include "ui/elements/dockPanel.hpp"
#include "ui/elements/stackPanel.hpp"
#include "ui/layout/bounding.hpp"

using paint::drawing::LineCap;
using paint::drawing::LineJoin;
using paint::drawing::Tool;

namespace paint::app {

PaintWindow::PaintWindow(Application& app, const std::string& title, int width, int height)
    : Window(app, title, width, height) {
    setupContent();
    setupMenu();
    setupShortcuts();

    updateToolStatus();

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (_shortcutManager.handle(event)) {
            event.stopPropagation();
            return;
        }
    });

    addEventListener<MouseMoveEvent>([this](MouseMoveEvent& event) {
        if (_canvas) {
            Point position = _canvas->windowToLocal(event.position());
            _positionText->setText("Pos: (" + std::to_string(position.x()) + ", " +
                                   std::to_string(position.y()) + ")");
        }
    });

    _canvas->addEventListener<ElementResizeEvent>([this](ElementResizeEvent& event) {
        if (_canvas) {
            Size size = _canvas->desiredSize();
            _sizeText->setText("Size: (" + std::to_string(size.width.value()) + ", " +
                               std::to_string(size.height.value()) + ")");
        }
    });
}

void PaintWindow::selectTool(drawing::Tool tool) {
    _canvas->setTool(tool);
    updateToolStatus();
}

void PaintWindow::setShapeStyle(const drawing::ShapeStyle& style) {
    _canvas->setStyle(style);
    updateToolStatus();
}

void PaintWindow::updateToolStatus() {
    if (_toolText) {
        _toolText->setText("Tool: " + drawing::getToolName(_canvas->currentTool()) +
                           " | Stroke: " + _canvas->style().stroke.color.toHexString() + " " +
                           std::to_string(_canvas->style().stroke.width) + "px" +
                           " | Fill: " + _canvas->style().fill.color.toHexString());
    }
}

void PaintWindow::setupContent() {
    auto dock = std::make_unique<ui::DockPanelElement>();

    auto statusBar = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);
    statusBar->setPadding({8, 4, 8, 4});

    auto canvas = std::make_unique<ui::CanvasElement>(_document);
    _canvas = canvas.get();

    // 設置狀態列的文字元素

    auto toolText =
        std::make_unique<ui::TextElement>("Tool: --", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    _toolText = toolText.get();
    // margin 保證最小間距；grow 讓工具欄吸收剩餘寬度，把其他欄位推向右側
    // Thickness 的順序是 left, top, right, bottom
    toolText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(toolText), 1.0f);

    auto positionText =
        std::make_unique<ui::TextElement>("Pos: (--, --)", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    _positionText = positionText.get();
    positionText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(positionText), 1.0f);

    auto sizeText =
        std::make_unique<ui::TextElement>("Size: (--, --)", BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12});
    _sizeText = sizeText.get();
    statusBar->appendChild(std::move(sizeText), 1.0f);

    // 設置 DockPanel 的內容

    dock->appendChild(std::move(statusBar), ui::Dock::Bottom);
    dock->appendChild(std::move(canvas));

    // 設置內容
    setContent(std::move(dock));
    setFocusedElement(_canvas);  // 將焦點設置為 CanvasElement
}

void PaintWindow::setupShortcuts() {
    // 設置快捷鍵
    _shortcutManager.bind({Mod::Primary, Key::Z}, [this]() { _canvas->undo(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::Z}, [this]() { _canvas->redo(); });

    _shortcutManager.bind({Mod::Primary, Key::S}, [this]() { _document.save(); });
    _shortcutManager.bind({Mod::Primary, Key::N}, [this]() { newFile(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::N}, [this]() { newWindow(); });
    _shortcutManager.bind({Mod::Primary, Key::C}, [this]() { close(); });
    _shortcutManager.bind({Mod::Primary, Key::R}, [this]() {
        requestCachedRedisplay();
        resetInputState();
    });
    _shortcutManager.bind({Key::F5}, [this]() { _canvas->clear(); });

    // 設置工具快捷鍵
    _shortcutManager.bind({Key::Digit1}, [this]() { selectTool(Tool::TOOL_PENCIL); });
    _shortcutManager.bind({Key::Digit2}, [this]() { selectTool(Tool::TOOL_LINE); });
    _shortcutManager.bind({Key::Digit3}, [this]() { selectTool(Tool::TOOL_RECTANGLE); });
    _shortcutManager.bind({Key::Digit4}, [this]() { selectTool(Tool::TOOL_ELLIPSE); });
    _shortcutManager.bind({Key::Digit5}, [this]() { selectTool(Tool::TOOL_POLYGON); });

    _shortcutManager.bind({Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(std::max(1, static_cast<int>(style.stroke.width - 1)));
        setShapeStyle(style);
    });
    _shortcutManager.bind({Key::RightBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(style.stroke.width + 1);
        setShapeStyle(style);
    });
    _shortcutManager.bind({Mod::Shift, Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(std::max(1, static_cast<int>(style.stroke.width - 5)));
        setShapeStyle(style);
    });
    _shortcutManager.bind({Mod::Shift, Key::RightBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(style.stroke.width + 5);
        setShapeStyle(style);
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

    auto& gridMenu = _menu.addSubMenu("Grid");
    gridMenu.addMenuEntry("Lines", [this]() { _canvas->setGridMode(ui::GridMode::Lines); });
    gridMenu.addMenuEntry("Dots", [this]() { _canvas->setGridMode(ui::GridMode::Dots); });
    gridMenu.addMenuEntry("None", [this]() { _canvas->setGridMode(ui::GridMode::None); });

    _menu.addMenuEntry("Close", [this]() { close(); });

    _menu.attach(MouseButton::MouseRight);
}

#pragma endregion  // Main Menu

#pragma region Tool Menu

void PaintWindow::setupToolMenu() {
    auto& toolMenu = _menu.addSubMenu("Tools");

    toolMenu.addMenuEntry("Select", [this]() {});
    toolMenu.addMenuEntry("Point", [this]() { selectTool(Tool::TOOL_POINT); });
    toolMenu.addMenuEntry("Pencil", [this]() { selectTool(Tool::TOOL_PENCIL); });
    toolMenu.addMenuEntry("Line", [this]() { selectTool(Tool::TOOL_LINE); });
    toolMenu.addMenuEntry("Rectangle", [this]() { selectTool(Tool::TOOL_RECTANGLE); });
    toolMenu.addMenuEntry("Circle / Ellipse", [this]() { selectTool(Tool::TOOL_ELLIPSE); });
    toolMenu.addMenuEntry("Polygon", [this]() { selectTool(Tool::TOOL_POLYGON); });
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
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::White);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Red);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Orange);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Yellow);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Green);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Blue);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->style();
        style.setStrokeColor(Color::Purple);
        setShapeStyle(style);
    });
    colorMenu.addMenuEntry("Custom...", [this]() {});

    // Stroke Width Menu

    auto& widthMenu = strokeMenu.addSubMenu("Width");

    widthMenu.addMenuEntry("1 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(1);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(3);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(5);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(10);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(25);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(50);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thicker", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 1);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thicker++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 3);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thicker+++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() + 5);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thinner", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 1);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thinner++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 3);
        setShapeStyle(style);
    });
    widthMenu.addMenuEntry("Thinner+++", [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(_canvas->style().strokeWidth() - 5);
        setShapeStyle(style);
    });

    // Stroke Join Menu

    auto& joinMenu = strokeMenu.addSubMenu("Join");

    joinMenu.addMenuEntry("Miter", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::MITER);
        setShapeStyle(style);
    });
    joinMenu.addMenuEntry("Bevel", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::BEVEL);
        setShapeStyle(style);
    });
    joinMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->style();
        style.setStrokeJoin(LineJoin::ROUND);
        setShapeStyle(style);
    });

    // Stroke Cap Menu

    auto& capMenu = strokeMenu.addSubMenu("Cap");

    capMenu.addMenuEntry("Round", [this]() {
        auto style = _canvas->style();
        style.setStrokeCap(LineCap::ROUND);
        setShapeStyle(style);
    });
    capMenu.addMenuEntry("Square", [this]() {
        auto style = _canvas->style();
        style.setStrokeCap(LineCap::SQUARE);
        setShapeStyle(style);
    });
    capMenu.addMenuEntry("Butt", [this]() {
        auto style = _canvas->style();
        style.setStrokeCap(LineCap::BUTT);
        setShapeStyle(style);
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
        setShapeStyle(style);
    });
    fillModeMenu.addMenuEntry("Filled", [this]() {
        auto style = _canvas->style();
        style.setFillMode(drawing::FillMode::FILLED);
        setShapeStyle(style);
    });
    fillModeMenu.addMenuEntry("Advanced", [this]() {
        auto style = _canvas->style();
        style.setFillMode(drawing::FillMode::ADVANCED);
        setShapeStyle(style);
    });

    // Fill Color Menu

    auto& fillColorMenu = fillMenu.addSubMenu("Color");

    fillColorMenu.addMenuEntry("Transparent", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Transparent);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Black", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Black);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::White);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Red);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Orange);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Yellow);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Green);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Blue);
        setShapeStyle(style);
    });
    fillColorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->style();
        style.setFillColor(Color::Purple);
        setShapeStyle(style);
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
        setShapeStyle(style);
    });
    pointMenu.addMenuEntry("3 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(3);
        setShapeStyle(style);
    });
    pointMenu.addMenuEntry("5 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(5);
        setShapeStyle(style);
    });
    pointMenu.addMenuEntry("10 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(10);
        setShapeStyle(style);
    });
    pointMenu.addMenuEntry("25 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(25);
        setShapeStyle(style);
    });
    pointMenu.addMenuEntry("50 px", [this]() {
        auto style = _canvas->style();
        style.setPointSize(50);
        setShapeStyle(style);
    });
}

#pragma endregion  // Point Menu

}  // namespace paint::app
