#include <GL/freeglut.h>

#include <app/application.hpp>
#include <iostream>
#include <window/window.hpp>

class TestWindow : public paint::Window {
   public:
    using Window::Window;  // 繼承父類的建構函數

   protected:
    void onKeyDown(const paint::KeyboardEvent& event) override {
        auto key = event.key();
        std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;
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
