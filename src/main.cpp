#include <GL/freeglut.h>

#include <iostream>

#include "app/application.hpp"
#include "menu/menu.hpp"
#include "window/window.hpp"

class TestWindow : public paint::Window {
   public:
    TestWindow(const std::string& title, int width, int height, bool resizable = true)
        : paint::Window(title, width, height, resizable) {
        setupMenu();
    }

   protected:
    void onKeyDown(const paint::KeyboardEvent& event) override {
        auto key = event.key();
        std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;
    }

    void onClick(const paint::MouseClickEvent& event) override {
        auto button = event.button();
        std::cout << "Mouse clicked: " << static_cast<int>(button) << std::endl;
    }

    void onDoubleClick(const paint::MouseClickEvent& event) override {
        auto button = event.button();
        std::cout << "Mouse double clicked: " << static_cast<int>(button) << std::endl;
    }

    void onDisplay() override {
        // 在這裡添加繪製代碼
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);  // Red
        glVertex2f(100.0f, 100.0f);
        glColor3f(0.0f, 1.0f, 0.0f);  // Green
        glVertex2f(100.0f, 600.0f);
        glColor3f(0.0f, 0.0f, 1.0f);  // Blue
        glVertex2f(600.0f, 100.0f);
        glEnd();
    }

   private:
    paint::Menu _menu;

    void setupMenu() {
        _menu.addMenuEntry("Option 1", []() { std::cout << "Option 1 selected" << std::endl; });
        _menu.addMenuEntry("Option 2", []() { std::cout << "Option 2 selected" << std::endl; });

        auto& submenu = _menu.addSubMenu("Submenu");
        submenu.addMenuEntry("Sub-option 1", []() { std::cout << "Sub-option 1 selected" << std::endl; });
        submenu.addMenuEntry("Sub-option 2", []() { std::cout << "Sub-option 2 selected" << std::endl; });

        _menu.attach(paint::MouseButton::MouseRight);
    }
};

int main(int argc, char** argv) {
    paint::Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<TestWindow>("Drawing Panel", 800, 600);

    app.createWindow<TestWindow>("Fixed Window", 400, 300, false);

    // 運行應用程序
    app.run();

    return 0;
}
