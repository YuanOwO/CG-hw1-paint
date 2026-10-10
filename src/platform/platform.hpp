#pragma once

namespace paint::platform {

// 接收視窗事件。每個函數對應一個 GLUT callback，參數也相同。
class WindowHandler {
   public:
    virtual ~WindowHandler() = default;

    virtual void onClose() = 0;                                      // glutCloseFunc
    virtual void onReshape(int width, int height) = 0;               // glutReshapeFunc
    virtual void onVisibility(int state) = 0;                        // glutVisibilityFunc
    virtual void onDisplay() = 0;                                    // glutDisplayFunc

    virtual void onKeyboard(unsigned char key, int x, int y) = 0;    // glutKeyboardFunc
    virtual void onKeyboardUp(unsigned char key, int x, int y) = 0;  // glutKeyboardUpFunc
    virtual void onSpecial(int key, int x, int y) = 0;               // glutSpecialFunc
    virtual void onSpecialUp(int key, int x, int y) = 0;             // glutSpecialUpFunc

    virtual void onMouse(int button, int state, int x, int y) = 0;   // glutMouseFunc
    virtual void onMotion(int x, int y) = 0;                         // glutMotionFunc
    virtual void onPassiveMotion(int x, int y) = 0;                  // glutPassiveMotionFunc
    virtual void onEntry(int state) = 0;                             // glutEntryFunc
};

// 取代 glutInit：初始化 GLUT，並設定關閉視窗時不結束程式
void initialize(int& argc, char** argv);

// 取代 glutMainLoop：所有視窗關閉後返回
void runMainLoop();

// 取代 glutGet(GLUT_INIT_STATE)：主循環結束後回傳 false，此時不能再呼叫 GLUT 函數
bool isRunning();

// 取代 glutCreateWindow 與上述的 glutXxxFunc：建立視窗，之後這個視窗的事件都會交給 handler。
// handler 必須在視窗銷毀前保持有效。
int createWindow(const char* title, WindowHandler& handler);

// 取代 glutDestroyWindow
void destroyWindow(int window);

}  // namespace paint::platform
