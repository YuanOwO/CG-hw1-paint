#pragma once

#include <string>

#include "app/document.hpp"
#include "input/shortcut_manager.hpp"
#include "ui/elements/canvas.hpp"
#include "ui/elements/text.hpp"
#include "ui/menu.hpp"
#include "ui/window.hpp"

namespace paint::app {

class Application;

class PaintWindow : public ui::Window {
   public:
    PaintWindow(Application& app, const std::string& title, int width, int height);

    ui::CanvasElement& canvas() { return *_canvas; }

   private:
    Document _document;
    ShortcutManager _shortcutManager;
    ui::Menu _menu;

    ui::CanvasElement* _canvas = nullptr;
    ui::TextElement* _toolText = nullptr;
    ui::TextElement* _positionText = nullptr;
    ui::TextElement* _sizeText = nullptr;

    void selectTool(drawing::Tool tool);
    void setShapeStyle(const drawing::ShapeStyle& style);
    void updateToolStatus();

    void setupContent();
    void setupShortcuts();

    void newFile();
    void newWindow();

    void setupMenu();

    void setupToolMenu();
    void setupStrokeMenu();
    void setupFillMenu();
    void setupPointMenu();
};

}  // namespace paint::app
