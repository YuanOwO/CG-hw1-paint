#pragma once

#include "menu/menu.hpp"
#include "window/window.hpp"

namespace paint {

class PaintWindow : public Window {
   public:
    PaintWindow(const std::string& title, int width, int height);

   protected:
    // void onDisplay() override;

    // void onKeyDown(const KeyboardEvent& event) override;

    // void onMouseDown(const MouseEvent& event) override;

    // void onMouseUp(const MouseEvent& event) override;

    // void onMouseMove(const MouseMoveEvent& event) override;

    // void onClick(const MouseClickEvent& event) override;

   private:
    Menu _menu;

    void setupMenu();
    void setupShapeMenu();
    void setupColorMenu();
    void setupFillColorMenu();
    void setupWidthMenu();
    void setupJoinMenu();
    void setupCapMenu();
};

}  // namespace paint
