#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "app/window.hpp"

namespace paint {

class Application {
   public:
    Application(int& argc, char** argv);

    ~Application() = default;

    // 禁止拷貝與移動操作，確保元素的唯一性
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // 運行應用程序，進入 GLUT 主循環
    void run();

    // 建立視窗，由 Application 管理生命週期，並回傳其參考
    template <typename WindowType, typename... Args>
    WindowType& createWindow(Args&&... args) {
        // 靜態斷言，確保 WindowType 是 Window 的衍生類
        static_assert(std::is_base_of_v<Window, WindowType>, "WindowType must derive from Window");

        auto window = std::make_unique<WindowType>(std::forward<Args>(args)...);

        WindowType& windowRef = *window;
        _windows.push_back(std::move(window));

        return windowRef;
    }

   private:
    std::vector<std::unique_ptr<Window>> _windows;  // 管理所有視窗的智能指針列表
};

}  // namespace paint
