// 程式在前景時使用英文鍵盤配置，說明見 input_source.hpp。

#ifdef __APPLE__

#  include "platform/apple/input_source.hpp"

#  include <Carbon/Carbon.h>

#  include "platform/apple/cocoa.hpp"

namespace paint::platform::input_source {

namespace {

// 切換前的輸入法；沒有切換時為 nullptr
TISInputSourceRef previousSource = nullptr;

bool isAsciiCapable(TISInputSourceRef source) {
    auto value =
        static_cast<CFBooleanRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceIsASCIICapable));
    return value != nullptr && CFBooleanGetValue(value);
}

// 程式切換到前景時：目前的輸入法無法輸入 ASCII 時，切換為 ASCII 鍵盤配置
void useAsciiSource() {
    if (previousSource != nullptr) {
        return;  // 已經切換過
    }

    TISInputSourceRef current = TISCopyCurrentKeyboardInputSource();
    if (current == nullptr) {
        return;
    }

    if (isAsciiCapable(current)) {
        CFRelease(current);
        return;
    }

    TISInputSourceRef ascii = TISCopyCurrentASCIICapableKeyboardLayoutInputSource();
    if (ascii != nullptr && TISSelectInputSource(ascii) == noErr) {
        previousSource = current;  // 保留，還原時使用
    } else {
        CFRelease(current);
    }

    if (ascii != nullptr) {
        CFRelease(ascii);
    }
}

}  // namespace

void install() {
    cocoa::observeActivation(useAsciiSource);
    cocoa::observeWillDeactivation(restore);
}

void restore() {
    if (previousSource == nullptr) {
        return;
    }

    TISSelectInputSource(previousSource);
    CFRelease(previousSource);
    previousSource = nullptr;
}

}  // namespace paint::platform::input_source

#endif
