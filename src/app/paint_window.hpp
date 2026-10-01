#pragma once

#include <string>

#include "document/document.hpp"
#include "input/shortcut_manager.hpp"
#include "ui/elements/canvas.hpp"
#include "ui/menu.hpp"
#include "ui/window.hpp"

namespace paint {

class PaintWindow : public ui::Window {
   public:
    PaintWindow(const std::string& title, int width, int height);

    ui::CanvasElement& canvas() { return *_canvas; }

   private:
    Document _document;
    ShortcutManager _shortcutManager;
    ui::Menu _menu;

    ui::CanvasElement* _canvas = nullptr;

    void setupMenu();

    void setupToolMenu();
    void setupStrokeMenu();
    void setupFillMenu();
    void setupPointMenu();
};

}  // namespace paint
