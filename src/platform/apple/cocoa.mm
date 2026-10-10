// 鍵盤事件攔截與 NSWindow 對照，說明見 cocoa.hpp。

#include "platform/apple/cocoa.hpp"

#include <GLUT/glut.h>

#import <Cocoa/Cocoa.h>

#include <iterator>
#include <unordered_map>

namespace paint::platform::cocoa {

namespace {

// NSWindow.windowNumber → GLUT 視窗
std::unordered_map<NSInteger, int> glutWindows;

int glutWindowOf(NSWindow* window) {
    if (window == nil) {
        return 0;
    }

    auto it = glutWindows.find(window.windowNumber);
    return it != glutWindows.end() ? it->second : 0;
}

KeyEvent toKeyEvent(NSEvent* event, NSWindow* window, int glutWindow) {
    KeyEvent result{};

    switch (event.type) {
    case NSEventTypeKeyDown:
        result.type = KeyEvent::Type::KeyDown;
        break;
    case NSEventTypeKeyUp:
        result.type = KeyEvent::Type::KeyUp;
        break;
    default:
        result.type = KeyEvent::Type::ModifiersChanged;
        break;
    }

    result.keyCode = event.keyCode;
    result.modifierFlags = event.modifierFlags;
    result.window = glutWindow;

    // GLUT 以左上角為原點。Apple GLUT 的視圖是翻轉的座標系統（原點在左上角），
    // 一般的 NSView 則以左下角為原點，需要翻轉 y
    NSView* view = window.contentView;
    const NSPoint point = [view convertPoint:window.mouseLocationOutsideOfEventStream fromView:nil];
    result.x = static_cast<int>(point.x);
    result.y = static_cast<int>(view.isFlipped ? point.y : view.bounds.size.height - point.y);

    return result;
}

}  // namespace

void installKeyEventMonitor(KeyEventHandler handler) {
    const NSEventMask mask = NSEventMaskKeyDown | NSEventMaskKeyUp | NSEventMaskFlagsChanged;

    [NSEvent addLocalMonitorForEventsMatchingMask:mask
                                          handler:^NSEvent*(NSEvent* event) {
                                              NSWindow* window = event.window ?: NSApp.keyWindow;
                                              const int glutWindow = glutWindowOf(window);
                                              if (glutWindow == 0) {
                                                  return event;
                                              }

                                              // 回傳 nil 表示事件已處理，Apple GLUT 不會收到
                                              return handler(toKeyEvent(event, window, glutWindow)) ? nil : event;
                                          }];
}

void observeDeactivation(void (*callback)()) {
    [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationDidResignActiveNotification
                                                      object:nil
                                                       queue:nil
                                                  usingBlock:^(NSNotification*) {
                                                      callback();
                                                  }];
}

void observeActivation(void (*callback)()) {
    [[NSNotificationCenter defaultCenter] addObserverForName:NSApplicationDidBecomeActiveNotification
                                                      object:nil
                                                       queue:nil
                                                  usingBlock:^(NSNotification*) {
                                                      callback();
                                                  }];

    if (NSApp.isActive) {
        callback();
    }
}

void observeWillDeactivation(void (*callback)()) {
    for (NSNotificationName name in @[ NSApplicationWillResignActiveNotification,
                                       NSApplicationWillTerminateNotification ]) {
        [[NSNotificationCenter defaultCenter] addObserverForName:name
                                                          object:nil
                                                           queue:nil
                                                      usingBlock:^(NSNotification*) {
                                                          callback();
                                                      }];
    }
}

int createWindow(const char* title) {
    // 比對建立前後的視窗清單，找出新視窗對應的 NSWindow
    NSSet* before = [NSSet setWithArray:NSApp.windows];
    const int glutWindow = glutCreateWindow(title);

    for (NSWindow* window in NSApp.windows) {
        if (![before containsObject:window]) {
            glutWindows[window.windowNumber] = glutWindow;
        }
    }

    return glutWindow;
}

void forgetWindow(int window) {
    for (auto it = glutWindows.begin(); it != glutWindows.end();) {
        it = it->second == window ? glutWindows.erase(it) : std::next(it);
    }
}

}  // namespace paint::platform::cocoa
