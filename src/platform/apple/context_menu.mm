// 停用右鍵選單的服務項目，說明見 cocoa.hpp。
//
// Apple GLUT 以 macOS 原生的右鍵選單顯示 GLUT 選單，macOS 會在後面附加其他 App 提供的服務項目。
// 這裡取代 Apple GLUT 內部的 -[GLUTView _popUpContextMenu:withEvent:]，在顯示前停用服務項目。

#include "platform/apple/cocoa.hpp"

#import <Cocoa/Cocoa.h>
#import <objc/message.h>
#import <objc/runtime.h>

#include <cstring>

namespace paint::platform::cocoa {

namespace {

IMP originalPopUpContextMenu = nullptr;

// menu 是 Apple GLUT 的 GLUTMenu，nativeMenu 回傳實際顯示的 NSMenu
void popUpContextMenu(id self, SEL selector, id menu, NSEvent* event) {
    const SEL nativeMenu = sel_registerName("nativeMenu");
    if ([menu respondsToSelector:nativeMenu]) {
        id native = reinterpret_cast<id (*)(id, SEL)>(objc_msgSend)(menu, nativeMenu);
        if ([native isKindOfClass:[NSMenu class]]) {
            static_cast<NSMenu*>(native).allowsContextMenuPlugIns = NO;
        }
    }

    reinterpret_cast<void (*)(id, SEL, id, NSEvent*)>(originalPopUpContextMenu)(self, selector, menu, event);
}

}  // namespace

void disableContextMenuPlugIns() {
    Method method =
        class_getInstanceMethod(NSClassFromString(@"GLUTView"), sel_registerName("_popUpContextMenu:withEvent:"));

    // 確認參數格式（兩個物件參數）相符才取代，避免 Apple GLUT 的內部實作改變時崩潰
    if (method && std::strcmp(method_getTypeEncoding(method), "v32@0:8@16@24") == 0) {
        originalPopUpContextMenu = method_setImplementation(method, reinterpret_cast<IMP>(popUpContextMenu));
    }
}

}  // namespace paint::platform::cocoa
