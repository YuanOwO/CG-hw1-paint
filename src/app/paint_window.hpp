#pragma once

#include <string>

#include "app/window.hpp"
#include "document/document.hpp"
#include "ui/elements.hpp"
#include "ui/menu.hpp"

namespace paint {

class PaintWindow : public Window {
   public:
    PaintWindow(const std::string& title, int width, int height);

    CanvasElement& getCanvas() { return *_canvas; }

   private:
    Document _document;
    Menu _menu;

    CanvasElement* _canvas = nullptr;

    void setupMenu();
    void setupShapeMenu();
    void setupColorMenu();
    void setupFillColorMenu();
    void setupWidthMenu();
    void setupJoinMenu();
    void setupCapMenu();
};

}  // namespace paint
