#include "app/windows/paint.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "app/application.hpp"
#include "app/windows/confirm.hpp"
#include "app/windows/input_dialog.hpp"
#include "common/font.hpp"
#include "ui/elements/dock_panel.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/layout/bounding.hpp"

using paint::drawing::LineCap;
using paint::drawing::LineJoin;
using paint::drawing::ToolKind;

namespace paint::app {
namespace {

std::string getFontStyleName(const FontStyle& style) {
    if (const auto* bitmap = std::get_if<BitmapFontStyle>(&style)) {
        switch (bitmap->font) {
        case BitmapFont::BITMAP_8_BY_13:
            return "8 x 13";
        case BitmapFont::BITMAP_9_BY_15:
            return "9 x 15";
        case BitmapFont::BITMAP_HELVETICA_10:
            return "Helvetica 10";
        case BitmapFont::BITMAP_HELVETICA_12:
            return "Helvetica 12";
        case BitmapFont::BITMAP_HELVETICA_18:
            return "Helvetica 18";
        case BitmapFont::BITMAP_TIMES_ROMAN_10:
            return "Times Roman 10";
        case BitmapFont::BITMAP_TIMES_ROMAN_24:
            return "Times Roman 24";
        default:
            return "Unknown";
        }
    }

    if (const auto* stroke = std::get_if<StrokeFontStyle>(&style)) {
        switch (stroke->font) {
        case StrokeFont::STROKE_ROMAN:
            return "Stroke Roman";
        case StrokeFont::STROKE_MONO_ROMAN:
            return "Stroke Mono Roman";
        default:
            return "Unknown";
        }
    }

    if (const auto* gfnt = std::get_if<GfntFontStyle>(&style)) {
        switch (gfnt->font) {
        case GfntFontId::CUBIC_11:
            return "俐方體11號";
        case GfntFontId::UNIFONT_16:
            return "Unifont 16";
        default:
            return "Unknown";
        }
    }

    return "Unknown";
}

}  // namespace

PaintWindow::PaintWindow(const std::string& title, int width, int height) : Window(title, width, height) {
    setupContent();
    setupMenu();
    setupShortcuts();

    updateToolStatus();

    _document.onModifiedChanged = [this](bool modified) {
        std::string title = Application::current().name();
        if (_document.filename().empty()) {
            title += " - Untitled";
        } else {
            title += " - " + _document.filename().string();
        }
        if (modified) {
            title += "*";
        }
        setTitle(title);
    };

    addEventListener<WindowCloseEvent>([this](WindowCloseEvent&) { requestClose(); });

    addEventListener<KeyDownEvent>([this](KeyDownEvent& event) {
        if (_shortcutManager.handle(event)) {
            event.stopPropagation();
            return;
        }
    });

    addEventListener<MouseMoveEvent>([this](MouseMoveEvent& event) {
        Point position = _canvas->windowToLocal(event.position());
        _positionText->setText("Pos: " + position.toString());
    });

    _canvas->addEventListener<ElementResizeEvent>([this](ElementResizeEvent& event) {
        _document.setCanvasSize(event.width(), event.height());
        _sizeText->setText("Size: " + Point(event.width(), event.height()).toString());
    });

    _canvas->setTextInputRequestHandler(
        [this](Point anchor, drawing::TextStyle style) { requestTextInput(anchor, std::move(style)); });
}

void PaintWindow::selectTool(drawing::ToolKind tool) {
    _canvas->setTool(tool);
    updateToolStatus();
}

void PaintWindow::setShapeStyle(const drawing::ShapeStyle& style) {
    _canvas->setStyle(style);
    updateToolStatus();
}

void PaintWindow::setTextStyle(const drawing::TextStyle& style) {
    _canvas->setTextStyle(style);
    updateToolStatus();
}

void PaintWindow::updateToolStatus() {
    if (_toolText) {
        if (_canvas->currentTool() == ToolKind::TEXT) {
            _toolText->setText("Tool: Text | Color: " + _canvas->textStyle().color.toHexString() +
                               " | Font: " + getFontStyleName(_canvas->textStyle().font));
            return;
        }

        _toolText->setText("Tool: " + drawing::getToolName(_canvas->currentTool()) +
                           " | Stroke: " + _canvas->style().stroke.color.toHexString() + " " +
                           std::to_string(_canvas->style().stroke.width) + "px" +
                           " | Fill: " + _canvas->style().fill.color.toHexString());
    }
}

void PaintWindow::requestTextInput(Point anchor, drawing::TextStyle style) {
    auto& app = Application::current();
    app.createWindow<InputDialogWindow>(
        "Insert Text", "請輸入文字：", "",
        [this, anchor, style = std::move(style)](const std::string& text) mutable {
            _canvas->insertText(anchor, text, std::move(style));
        });
}

void PaintWindow::requestNewFile() {
    // 在創建新文件前，檢查是否有未保存的更改
    if (_document.isModified()) {
        auto& app = app::Application::current();
        app.createWindow<ConfirmWindow>("Unsaved Changes",
                                        "你有未保存的更改。\n你確定要創建一個新的檔案而不保存嗎？",
                                        [this]() { newFile(); });
    } else {
        newFile();  // 直接創建新文件，因為沒有未保存的更改
    }
}

void PaintWindow::requestLoadFile() {
    // 在加載文件前，檢查是否有未保存的更改
    if (_document.isModified()) {
        auto& app = app::Application::current();
        app.createWindow<ConfirmWindow>("Unsaved Changes",
                                        "你有未保存的更改。\n你確定要開啟一個新的檔案而不保存嗎？",
                                        [this]() { loadFile(); });
    } else {
        loadFile();  // 直接加載文件，因為沒有未保存的更改
    }
}

void PaintWindow::requestClose() {
    // 在關閉窗口前，檢查是否有未保存的更改
    if (_document.isModified()) {
        auto& app = app::Application::current();
        app.createWindow<ConfirmWindow>("Unsaved Changes", "你有未保存的更改。\n你確定要關閉而不保存嗎？",
                                        [this]() { close(); });
    } else {
        close();  // 直接關閉窗口，因為沒有未保存的更改
    }
}

void PaintWindow::newFile() {
    _document.newFile();
    requestRedisplay();
}

void PaintWindow::loadFile() {
    const std::string initialFilename = _document.filename().empty() ? "" : _document.filename().string();

    auto& app = app::Application::current();
    app.createWindow<InputDialogWindow>(
        "Open", "請輸入文件名：", initialFilename,
        [this](const std::string& value) {
            Path filename(value);
            if (!filename.has_extension()) {
                filename += ".gpt";
            }

            _document.load(filename);
            requestRedisplay();
        },
        nullptr,
        [](const std::string& value) -> std::optional<std::string> {
            Path filename(value);
            if (!filename.has_extension()) {
                filename += ".gpt";
            }

            std::error_code error;
            const bool exists = std::filesystem::exists(filename, error);
            if (error) {
                return "無法訪問該檔案。";
            }
            if (!exists || !std::filesystem::is_regular_file(filename, error)) {
                return "檔案 \"" + filename.string() + "\" 不存在。";
            }
            if (error) {
                return "無法訪問該檔案。";
            }

            return std::nullopt;
        });
}

void PaintWindow::saveFile() {
    if (_document.filename().empty()) {
        saveFileAs();  // 如果沒有文件名，則調用另存為
    } else {
        _document.save();
    }
}

void PaintWindow::saveFileAs() {
    const std::string initialFilename =
        _document.filename().empty() ? "untitled.gpt" : _document.filename().filename().string();

    auto& app = app::Application::current();
    app.createWindow<InputDialogWindow>(
        "Save As", "請輸入文件名：", initialFilename, [this](const std::string& value) {
            Path filename(value);

            if (!filename.has_extension()) {
                filename += ".gpt";
            }

            if (!std::filesystem::exists(filename)) {
                _document.save(filename);
                return;
            }

            auto& app = Application::current();

            app.createWindow<ConfirmWindow>(
                "File Already Exists", "檔案 \"" + filename.string() + "\" 已經存在。\n你確定要覆蓋它嗎？",
                [this, filename]() { _document.save(filename); });
        });
}

void PaintWindow::exportFile() {
    const std::string initialFilename =
        _document.filename().empty() ? "untitled.ppm" : _document.filename().stem().string() + ".ppm";

    auto& app = app::Application::current();
    app.createWindow<InputDialogWindow>(
        "Export", "請輸入圖像檔案名稱：", initialFilename, [this](const std::string& value) {
            Path filename(value);

            if (!filename.has_extension()) {
                filename += ".ppm";
            }

            if (!std::filesystem::exists(filename)) {
                _document.exportImage(filename);
                return;
            }

            auto& app = Application::current();
            app.createWindow<ConfirmWindow>(
                "File Already Exists", "檔案 \"" + filename.string() + "\" 已經存在。\n你確定要覆蓋它嗎？",
                [this, filename]() { _document.exportImage(filename); });
        });
}

void PaintWindow::newWindow() {
    auto& app = app::Application::current();
    app.createWindow<PaintWindow>(app.name() + " - Untitled", width(), height());
}

#pragma region Content Setup

void PaintWindow::setupContent() {
    auto dock = std::make_unique<ui::DockPanelElement>();

    auto statusBar = std::make_unique<ui::StackPanelElement>(ui::StackOrientation::Horizontal);
    statusBar->setBackgroundColor(Color::LightGray);
    statusBar->setPadding({8, 4, 8, 4});

    auto canvas = std::make_unique<ui::CanvasElement>(_document);
    _canvas = canvas.get();

    // 設置狀態列的文字元素

    auto toolText = std::make_unique<ui::TextElement>("Tool: --", GfntFontStyle{GfntFontId::CUBIC_11});
    _toolText = toolText.get();
    // margin 保證最小間距；grow 讓工具欄吸收剩餘寬度，把其他欄位推向右側
    // Thickness 的順序是 left, top, right, bottom
    toolText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(toolText), 1.0f);

    auto positionText =
        std::make_unique<ui::TextElement>("Pos: (--, --)", GfntFontStyle{GfntFontId::CUBIC_11});
    _positionText = positionText.get();
    positionText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(positionText), 1.0f);

    auto sizeText = std::make_unique<ui::TextElement>("Size: (--, --)", GfntFontStyle{GfntFontId::CUBIC_11});
    _sizeText = sizeText.get();
    statusBar->appendChild(std::move(sizeText), 1.0f);

    // 設置 DockPanel 的內容

    dock->appendChild(std::move(statusBar), ui::Dock::Bottom);
    dock->appendChild(std::move(canvas));

    // 設置內容
    setContent(std::move(dock));
    setFocusedElement(_canvas);  // 將焦點設置為 CanvasElement
}

#pragma endregion  // Content Setup

#pragma region Shortcuts

void PaintWindow::setupShortcuts() {
    // 設置快捷鍵
    _shortcutManager.bind({Mod::Primary, Key::Z}, [this]() { _canvas->undo(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::Z}, [this]() { _canvas->redo(); });
    _shortcutManager.bind({Key::C}, [this]() { _canvas->clear(); });
    _shortcutManager.bind({Key::G}, [this]() {
        auto gridMode = _canvas->gridMode();
        if (gridMode == ui::GridMode::None) {
            _canvas->setGridMode(ui::GridMode::Lines);
        } else if (gridMode == ui::GridMode::Lines) {
            _canvas->setGridMode(ui::GridMode::Dots);
        } else if (gridMode == ui::GridMode::Dots) {
            _canvas->setGridMode(ui::GridMode::None);
        }
    });

    _shortcutManager.bind({Mod::Primary, Key::N}, [this]() { requestNewFile(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::N}, [this]() { newWindow(); });
    _shortcutManager.bind({Mod::Primary, Key::O}, [this]() { requestLoadFile(); });
    _shortcutManager.bind({Mod::Primary, Key::S}, [this]() { saveFile(); });
    _shortcutManager.bind({Mod::Primary, Mod::Shift, Key::S}, [this]() { saveFileAs(); });
    _shortcutManager.bind({Mod::Primary, Key::E}, [this]() { exportFile(); });
    // _shortcutManager.bind({Mod::Primary, Key::C}, [this]() { requestClose(); });
    _shortcutManager.bind({Mod::Primary, Key::R}, [this]() {
        requestCachedRedisplay();
        resetInputState();
    });

    // 設置工具快捷鍵
    _shortcutManager.bind({Key::Digit0}, [this]() { selectTool(ToolKind::SELECT); });
    _shortcutManager.bind({Key::Digit1}, [this]() { selectTool(ToolKind::PENCIL); });
    _shortcutManager.bind({Key::Digit2}, [this]() { selectTool(ToolKind::LINE); });
    _shortcutManager.bind({Key::Digit3}, [this]() { selectTool(ToolKind::RECTANGLE); });
    _shortcutManager.bind({Key::Digit4}, [this]() { selectTool(ToolKind::ELLIPSE); });
    _shortcutManager.bind({Key::Digit5}, [this]() { selectTool(ToolKind::POLYGON); });
    _shortcutManager.bind({Key::Digit6}, [this]() { selectTool(ToolKind::TEXT); });

    _shortcutManager.bind({Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(std::max(1, static_cast<int>(style.stroke.width - 1)));
        style.setPointSize(std::max(1.0f, style.pointSize - 1.0f));
        setShapeStyle(style);
    });
    _shortcutManager.bind({Key::RightBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(style.stroke.width + 1);
        style.setPointSize(style.pointSize + 1.0f);
        setShapeStyle(style);
    });
    _shortcutManager.bind({Mod::Shift, Key::LeftBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(std::max(1, static_cast<int>(style.stroke.width - 5)));
        style.setPointSize(std::max(1.0f, style.pointSize - 5.0f));
        setShapeStyle(style);
    });
    _shortcutManager.bind({Mod::Shift, Key::RightBracket}, [this]() {
        auto style = _canvas->style();
        style.setStrokeWidth(style.stroke.width + 5);
        style.setPointSize(style.pointSize + 5.0f);
        setShapeStyle(style);
    });
}

#pragma endregion  // Shortcuts

#pragma region Main Menu

void PaintWindow::setupMenu() {
    auto& fileMenu = _menu.addSubMenu("File");
    fileMenu.addMenuEntry("New", [this]() { requestNewFile(); });
    fileMenu.addMenuEntry("New Window", [this]() { newWindow(); });
    fileMenu.addMenuEntry("Load", [this]() { requestLoadFile(); });
    fileMenu.addMenuEntry("Save", [this]() { saveFile(); });
    fileMenu.addMenuEntry("Save As", [this]() { saveFileAs(); });
    fileMenu.addMenuEntry("Export", [this]() { exportFile(); });

    auto& editMenu = _menu.addSubMenu("Edit");
    editMenu.addMenuEntry("Undo", [this]() { _canvas->undo(); });
    editMenu.addMenuEntry("Redo", [this]() { _canvas->redo(); });
    editMenu.addMenuEntry("Clear", [this]() { _canvas->clear(); });

    setupToolMenu();
    setupStrokeMenu();
    setupFillMenu();
    setupPointMenu();
    setupTextMenu();

    auto& gridMenu = _menu.addSubMenu("Grid");
    gridMenu.addMenuEntry("Lines", [this]() { _canvas->setGridMode(ui::GridMode::Lines); });
    gridMenu.addMenuEntry("Dots", [this]() { _canvas->setGridMode(ui::GridMode::Dots); });
    gridMenu.addMenuEntry("None", [this]() { _canvas->setGridMode(ui::GridMode::None); });

    _menu.addMenuEntry("Close", [this]() { requestClose(); });

    _menu.attach(MouseButton::MouseRight);
}

#pragma endregion  // Main Menu

#pragma region Tool Menu

void PaintWindow::setupToolMenu() {
    auto& toolMenu = _menu.addSubMenu("Tools");

    toolMenu.addMenuEntry("Select", [this]() { selectTool(ToolKind::SELECT); });
    toolMenu.addMenuEntry("Point", [this]() { selectTool(ToolKind::POINT); });
    toolMenu.addMenuEntry("Pencil", [this]() { selectTool(ToolKind::PENCIL); });
    toolMenu.addMenuEntry("Line", [this]() { selectTool(ToolKind::LINE); });
    toolMenu.addMenuEntry("Rectangle", [this]() { selectTool(ToolKind::RECTANGLE); });
    toolMenu.addMenuEntry("Circle / Ellipse", [this]() { selectTool(ToolKind::ELLIPSE); });
    toolMenu.addMenuEntry("Polygon", [this]() { selectTool(ToolKind::POLYGON); });
    toolMenu.addMenuEntry("Text", [this]() { selectTool(ToolKind::TEXT); });
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

#pragma region Text Menu

void PaintWindow::setupTextMenu() {
    auto& textMenu = _menu.addSubMenu("Text");
    auto& fontMenu = textMenu.addSubMenu("Font");

    fontMenu.addMenuEntry("8 x 13", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_8_BY_13};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("9 x 15", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_9_BY_15};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Helvetica 10", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_10};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Helvetica 12", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Helvetica 18", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Times Roman 10", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_10};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Times Roman 24", [this]() {
        auto style = _canvas->textStyle();
        style.font = BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_24};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Cubic 11", [this]() {
        auto style = _canvas->textStyle();
        style.font = GfntFontStyle{GfntFontId::CUBIC_11};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Unifont 16", [this]() {
        auto style = _canvas->textStyle();
        style.font = GfntFontStyle{GfntFontId::UNIFONT_16};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Stroke Roman", [this]() {
        auto style = _canvas->textStyle();
        style.font = StrokeFontStyle{StrokeFont::STROKE_ROMAN, 0.15f};
        setTextStyle(style);
    });
    fontMenu.addMenuEntry("Stroke Mono Roman", [this]() {
        auto style = _canvas->textStyle();
        style.font = StrokeFontStyle{StrokeFont::STROKE_MONO_ROMAN, 0.15f};
        setTextStyle(style);
    });

    auto& colorMenu = textMenu.addSubMenu("Color");

    colorMenu.addMenuEntry("Black", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Black;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("White", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::White;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Red", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Red;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Orange", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Orange;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Yellow", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Yellow;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Green", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Green;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Blue", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Blue;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Purple", [this]() {
        auto style = _canvas->textStyle();
        style.color = Color::Purple;
        setTextStyle(style);
    });
    colorMenu.addMenuEntry("Custom...", [this]() {});
}

#pragma endregion  // Text Menu

}  // namespace paint::app
