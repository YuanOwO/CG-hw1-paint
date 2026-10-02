#include <GL/freeglut.h>

#include "app/application.hpp"
#include "app/windows/paint.hpp"

using namespace paint::app;

int main(int argc, char** argv) {
    Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<PaintWindow>(app.name(), 800, 600);
    app.createWindow<TestWindow>("Test Window", 400, 300);

    // 運行應用程序
    app.run();

    return 0;
}
