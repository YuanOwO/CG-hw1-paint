#include <filesystem>

#include "app/application.hpp"
#include "app/windows/paint.hpp"
#include "common/font.hpp"

using namespace paint;
using namespace paint::app;

int main(int argc, char** argv) {
    const auto executablePath = std::filesystem::weakly_canonical(std::filesystem::absolute(argv[0]));
    initializeFonts(executablePath.parent_path() / "assets" / "fonts");

    Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<PaintWindow>(app.name() + " - Untitled", 800, 600);

    // 運行應用程序
    app.run();

    return 0;
}
