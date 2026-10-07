#pragma once

#include <string>

#include "app/document.hpp"
#include "common/point.hpp"
#include "drawing/paint_style.hpp"
#include "drawing/text_style.hpp"
#include "drawing/tools/tool_factory.hpp"
#include "input/shortcut_manager.hpp"
#include "ui/elements/canvas.hpp"
#include "ui/elements/text.hpp"
#include "ui/menu.hpp"
#include "ui/window.hpp"

namespace paint::app {

class Application;

class PaintWindow : public ui::Window {
   public:
    PaintWindow(const std::string& title, int width, int height);

    ui::CanvasElement& canvas() { return *_canvas; }

   private:
    Document _document;
    ShortcutManager _shortcutManager;
    ui::Menu _menu;

    ui::CanvasElement* _canvas = nullptr;
    ui::TextElement* _toolText = nullptr;
    ui::TextElement* _positionText = nullptr;
    ui::TextElement* _sizeText = nullptr;

    void selectTool(drawing::ToolKind tool);
    void updateToolStatus();
    void requestTextInput(Point anchor, drawing::PaintStyle paint, drawing::TextStyle style);

    void requestNewFile();
    void requestOpenFile();
    void requestClose();

    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void exportFile();
    void newWindow();

    void setupContent();
    void setupShortcuts();

    void setupMenu();

    void setupToolMenu();
    void setupColorMenu();
    void setupStrokeMenu();
    void setupFillMenu();
    void setupPointMenu();
    void setupTextMenu();
};

}  // namespace paint::app
