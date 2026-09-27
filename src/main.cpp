#include <GL/freeglut.h>

#include "app/application.hpp"
#include "app/paintWindow.hpp"

int main(int argc, char** argv) {
    paint::Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<paint::PaintWindow>("Drawing Panel", 800, 600);

    // 運行應用程序
    app.run();

    return 0;
}
