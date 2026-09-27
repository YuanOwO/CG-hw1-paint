#include <app/application.hpp>
#include <iostream>
#include <window/window.hpp>

class TestWindow : public paint::Window {
   public:
    using Window::Window;  // 繼承父類的建構函數

   protected:
    void onKeyDown(const paint::KeyboardEvent& event) override {
        auto key = event.button();
        std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;
    }
};

int main(int argc, char** argv) {
    paint::Application app(argc, argv);

    // 在這裡創建視窗
    app.createWindow<TestWindow>("My Window", 800, 600);

    // 運行應用程序
    app.run();

    return 0;
}
