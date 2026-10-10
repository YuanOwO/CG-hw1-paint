#pragma once

// 程式在前景時使用英文鍵盤配置。
//
// GLUT 的鍵盤 callback 只有一個 unsigned char，無法輸入中文；
// 注音等輸入法開啟時，Apple GLUT 會送出輸入法的字元，讓單鍵快捷鍵失效。
// 因此程式切換到前景時改用 ASCII 鍵盤配置（例如 ABC），切換到背景或結束時還原。

namespace paint::platform::input_source {

// 開始在前景時使用英文鍵盤配置
void install();

// 還原為切換前的輸入法
void restore();

}  // namespace paint::platform::input_source
