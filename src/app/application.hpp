#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "ui/window.hpp"

namespace paint::app {

class Application {
   public:
    Application(int& argc, char** argv);

    ~Application();

    // 禁止拷貝與移動操作，確保元素的唯一性
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    static Application& current();
    static std::string version() { return "0.0.1"; }
    static std::string name() {
        // 想法：
        // Pictorium, Graphtoria, Graphium, Graphorium
        return "Graphtoria";
    }

    // 運行應用程序，進入 GLUT 主循環
    void run();

    // 建立視窗，由 Application 管理生命週期，並回傳其參考
    template <typename WindowType, typename... Args>
    WindowType& createWindow(Args&&... args) {
        // 靜態斷言，確保 WindowType 是 Window 的衍生類
        static_assert(std::is_base_of_v<ui::Window, WindowType>, "WindowType must derive from Window");

        auto window = std::make_unique<WindowType>(*this, std::forward<Args>(args)...);

        WindowType& windowRef = *window;
        _windows.push_back(std::move(window));

        return windowRef;
    }

   private:
    std::vector<std::unique_ptr<ui::Window>> _windows;  // 管理所有視窗的智能指針列表

    static bool _instanceExists;  // 用於確保 Application 的唯一性

    void removeClosedWindows();

    static void idleCallback();
};

}  // namespace paint::app
