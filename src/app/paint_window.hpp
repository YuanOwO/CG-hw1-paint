#pragma once

#include "element/canvas.hpp"
#include "menu/menu.hpp"
#include "window/window.hpp"

namespace paint {

class PaintWindow : public Window {
   public:
    PaintWindow(const std::string& title, int width, int height);

    CanvasElement& getCanvas() { return *_canvas; }

   private:
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
