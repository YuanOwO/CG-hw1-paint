#include "app/windows/paint.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "app/application.hpp"
#include "app/windows/color_picker.hpp"
#include "app/windows/confirm.hpp"
#include "app/windows/input_dialog.hpp"
#include "common/font.hpp"
#include "ui/elements/dock_panel.hpp"
#include "ui/elements/stack_panel.hpp"
#include "ui/layout/bounding.hpp"
#include "ui/theme.hpp"

using paint::drawing::FillMode;
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

// 主要顏色與填入顏色選單共用的色票。
const std::pair<const char*, Color> kMenuColors[] = {
    {"Black",  Color::Black },
    {"White",  Color::White },
    {"Red",    Color::Red   },
    {"Orange", Color::Orange},
    {"Yellow", Color::Yellow},
    {"Green",  Color::Green },
    {"Blue",   Color::Blue  },
    {"Purple", Color::Purple},
};

// 線寬與點大小選單共用的預設尺寸。
const float kSizePresets[] = {1.0f, 3.0f, 5.0f, 10.0f, 25.0f, 50.0f};

std::string formatPixels(float size) {
    return std::to_string(static_cast<int>(size)) + " px";
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
        _positionText->setText("Pos: " + formatPixels(position.x()) + ", " + formatPixels(position.y()));
    });

    _canvas->addEventListener<ElementResizeEvent>([this](ElementResizeEvent& event) {
        _document.setCanvasSize(event.width(), event.height());
        _sizeText->setText("Size: " + formatPixels(event.width()) + " x " + formatPixels(event.height()));
    });

    _canvas->setTextInputRequestHandler(
        [this](Point anchor, drawing::PaintStyle paint, drawing::TextStyle style) {
            requestTextInput(anchor, paint, std::move(style));
        });
    _canvas->setStyleChangedHandler([this](const drawing::StyleSet&) { updateToolStatus(); });
}

void PaintWindow::selectTool(drawing::ToolKind tool) {
    _canvas->setTool(tool);
    updateToolStatus();
}

void PaintWindow::updateToolStatus() {
    if (!_toolText) {
        return;
    }

    const auto& style = _canvas->currentStyle();

    if (_canvas->currentTool() == ToolKind::SELECT) {
        _toolText->setText("Tool: Select");
        return;
    }

    if (_canvas->currentTool() == ToolKind::TEXT) {
        _toolText->setText("Tool: Text | Color: " + style.paint.color.toHexString() +
                           " | Font: " + getFontStyleName(style.text.font));
        return;
    }

    if (_canvas->currentTool() == ToolKind::POINT) {
        _toolText->setText("Tool: " + drawing::getToolName(_canvas->currentTool()) +
                           " | Color: " + style.paint.color.toHexString() + " " +
                           std::to_string(style.shape.stroke.width) + "px");
        return;
    }

    _toolText->setText("Tool: " + drawing::getToolName(_canvas->currentTool()) + " | Color: " +
                       style.paint.color.toHexString() + " " + std::to_string(style.shape.stroke.width) +
                       "px | Fill: " + style.paint.fillColor.toHexString());
}

void PaintWindow::requestTextInput(Point anchor, drawing::PaintStyle paint, drawing::TextStyle style) {
    auto& app = Application::current();
    app.createWindow<InputDialogWindow>(
        "Insert Text", "請輸入文字：", "",
        [this, anchor, paint, style = std::move(style)](const std::string& text) mutable {
            _canvas->insertText(anchor, text, paint, std::move(style));
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

    auto toolText = std::make_unique<ui::TextElement>("Tool: --", ui::theme::BodyFont);
    _toolText = toolText.get();
    // margin 保證最小間距；grow 讓工具欄吸收剩餘寬度，把其他欄位推向右側
    // Thickness 的順序是 left, top, right, bottom
    toolText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(toolText), 1.0f);

    auto positionText = std::make_unique<ui::TextElement>("Pos: (--, --)", ui::theme::BodyFont);
    _positionText = positionText.get();
    positionText->setMargin({0, 0, 16, 0});
    statusBar->appendChild(std::move(positionText), 1.0f);

    auto sizeText = std::make_unique<ui::TextElement>("Size: (--, --)", ui::theme::BodyFont);
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

    // 調整線寬與點大小，兩者皆最小為 1
    const auto adjustSize = [this](float delta) {
        auto style = _canvas->currentStyle();
        style.shape.stroke.setWidth(style.shape.stroke.width + delta);
        style.shape.setPointSize(style.shape.pointSize + delta);
        _canvas->setCurrentStyle(style);
    };
    _shortcutManager.bind({Key::LeftBracket}, [adjustSize]() { adjustSize(-1.0f); });
    _shortcutManager.bind({Key::RightBracket}, [adjustSize]() { adjustSize(1.0f); });
    _shortcutManager.bind({Mod::Shift, Key::LeftBracket}, [adjustSize]() { adjustSize(-5.0f); });
    _shortcutManager.bind({Mod::Shift, Key::RightBracket}, [adjustSize]() { adjustSize(5.0f); });
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
    setupColorMenu();
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

#pragma region Color Menu

void PaintWindow::setupColorMenu() {
    auto& colorMenu = _menu.addSubMenu("Color");

    auto& primaryColorMenu = colorMenu.addSubMenu("Primary Color");
    for (const auto& [name, color] : kMenuColors) {
        primaryColorMenu.addMenuEntry(name, [this, color = color]() {
            auto style = _canvas->currentStyle();
            style.paint.color = color;
            _canvas->setCurrentStyle(style);
        });
    }
    primaryColorMenu.addMenuEntry("Custom...", [this]() {
        const ColorRGBA initialColor = _canvas->currentStyle().paint.color;
        Application::current().createWindow<ColorPickerWindow>("Primary Color", initialColor,
                                                               [this](const ColorRGBA& color) {
                                                                   auto style = _canvas->currentStyle();
                                                                   style.paint.color = color;
                                                                   _canvas->setCurrentStyle(style);
                                                               });
    });

    auto& fillColorMenu = colorMenu.addSubMenu("Fill Color");
    fillColorMenu.addMenuEntry("Transparent", [this]() {
        auto style = _canvas->currentStyle();
        style.paint.fillColor = Color::Transparent;
        _canvas->setCurrentStyle(style);
    });
    for (const auto& [name, color] : kMenuColors) {
        fillColorMenu.addMenuEntry(name, [this, color = color]() {
            auto style = _canvas->currentStyle();
            style.paint.fillColor = color;
            _canvas->setCurrentStyle(style);
        });
    }
    fillColorMenu.addMenuEntry("Custom...", [this]() {
        const ColorRGBA initialColor = _canvas->currentStyle().paint.fillColor;
        Application::current().createWindow<ColorPickerWindow>("Fill Color", initialColor,
                                                               [this](const ColorRGBA& color) {
                                                                   auto style = _canvas->currentStyle();
                                                                   style.paint.fillColor = color;
                                                                   _canvas->setCurrentStyle(style);
                                                               });
    });
}

#pragma endregion  // Color Menu

#pragma region Stroke Menu

void PaintWindow::setupStrokeMenu() {
    auto& strokeMenu = _menu.addSubMenu("Stroke");

    // Stroke Width Menu

    auto& widthMenu = strokeMenu.addSubMenu("Width");

    for (float width : kSizePresets) {
        widthMenu.addMenuEntry(formatPixels(width), [this, width]() {
            auto style = _canvas->currentStyle();
            style.shape.stroke.setWidth(width);
            _canvas->setCurrentStyle(style);
        });
    }

    const std::pair<const char*, float> widthSteps[] = {
        {"Thicker",    1.0f },
        {"Thicker++",  3.0f },
        {"Thicker+++", 5.0f },
        {"Thinner",    -1.0f},
        {"Thinner++",  -3.0f},
        {"Thinner+++", -5.0f},
    };
    for (const auto& [name, delta] : widthSteps) {
        widthMenu.addMenuEntry(name, [this, delta = delta]() {
            auto style = _canvas->currentStyle();
            style.shape.stroke.setWidth(style.shape.stroke.width + delta);
            _canvas->setCurrentStyle(style);
        });
    }

    // Stroke Join Menu

    auto& joinMenu = strokeMenu.addSubMenu("Join");

    const std::pair<const char*, LineJoin> joins[] = {
        {"Miter", LineJoin::MITER},
        {"Bevel", LineJoin::BEVEL},
        {"Round", LineJoin::ROUND},
    };
    for (const auto& [name, join] : joins) {
        joinMenu.addMenuEntry(name, [this, join = join]() {
            auto style = _canvas->currentStyle();
            style.shape.stroke.join = join;
            _canvas->setCurrentStyle(style);
        });
    }

    // Stroke Cap Menu

    auto& capMenu = strokeMenu.addSubMenu("Cap");

    const std::pair<const char*, LineCap> caps[] = {
        {"Round",  LineCap::ROUND },
        {"Square", LineCap::SQUARE},
        {"Butt",   LineCap::BUTT  },
    };
    for (const auto& [name, cap] : caps) {
        capMenu.addMenuEntry(name, [this, cap = cap]() {
            auto style = _canvas->currentStyle();
            style.shape.stroke.cap = cap;
            _canvas->setCurrentStyle(style);
        });
    }
}

#pragma endregion  // Stroke Menu

#pragma region Fill Menu

void PaintWindow::setupFillMenu() {
    auto& fillMenu = _menu.addSubMenu("Fill");

    // Fill Mode Menu

    auto& fillModeMenu = fillMenu.addSubMenu("Mode");

    const std::pair<const char*, FillMode> modes[] = {
        {"Outline",  FillMode::OUTLINE },
        {"Filled",   FillMode::FILLED  },
        {"Advanced", FillMode::ADVANCED},
    };
    for (const auto& [name, mode] : modes) {
        fillModeMenu.addMenuEntry(name, [this, mode = mode]() {
            auto style = _canvas->currentStyle();
            style.shape.fillMode = mode;
            _canvas->setCurrentStyle(style);
        });
    }
}

#pragma endregion  // Fill Menu

#pragma region Point Menu

void PaintWindow::setupPointMenu() {
    auto& pointMenu = _menu.addSubMenu("Point");

    for (float size : kSizePresets) {
        pointMenu.addMenuEntry(formatPixels(size), [this, size]() {
            auto style = _canvas->currentStyle();
            style.shape.setPointSize(size);
            _canvas->setCurrentStyle(style);
        });
    }
}

#pragma endregion  // Point Menu

#pragma region Text Menu

void PaintWindow::setupTextMenu() {
    auto& fontMenu = _menu.addSubMenu("Text Font");

    const std::pair<const char*, FontStyle> fonts[] = {
        {"8 x 13",            BitmapFontStyle{BitmapFont::BITMAP_8_BY_13}          },
        {"9 x 15",            BitmapFontStyle{BitmapFont::BITMAP_9_BY_15}          },
        {"Helvetica 10",      BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_10}     },
        {"Helvetica 12",      BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12}     },
        {"Helvetica 18",      BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18}     },
        {"Times Roman 10",    BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_10}   },
        {"Times Roman 24",    BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_24}   },
        {"Cubic 11",          GfntFontStyle{GfntFontId::CUBIC_11}                  },
        {"Unifont 16",        GfntFontStyle{GfntFontId::UNIFONT_16}                },
        {"Stroke Roman",      StrokeFontStyle{StrokeFont::STROKE_ROMAN, 0.15f}     },
        {"Stroke Mono Roman", StrokeFontStyle{StrokeFont::STROKE_MONO_ROMAN, 0.15f}},
    };
    for (const auto& [name, font] : fonts) {
        fontMenu.addMenuEntry(name, [this, font = font]() {
            auto style = _canvas->currentStyle();
            style.text.font = font;
            _canvas->setCurrentStyle(style);
        });
    }
}

#pragma endregion  // Text Menu

}  // namespace paint::app
