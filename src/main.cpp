#include <GL/freeglut.h>

#include <iostream>

#include "app/application.hpp"
#include "app/windows/confirm.hpp"
#include "app/windows/paint.hpp"

using namespace paint::app;

int main(int argc, char** argv) {
    Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<PaintWindow>(app.name(), 800, 600);
    app.createWindow<ConfirmWindow>(
        "Confirm", "Are you sure?",
        []() {
            // Confirm callback
            std::cout << "Confirm pressed" << std::endl;
        },
        []() {
            // Cancel callback
            std::cout << "Cancel pressed" << std::endl;
        });

    // 運行應用程序
    app.run();

    return 0;
}
