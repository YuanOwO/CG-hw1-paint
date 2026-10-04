#pragma once

#include <string>

#include "app/document.hpp"

namespace paint::io {

class DocumentStorage {
   public:
    // 寫入失敗拋出例外。
    static void write(const app::DocumentData& document);

    // 讀取、解析、驗證成功後才回傳；失敗拋出例外。
    static app::DocumentData read(const Path& filename);
};

}  // namespace paint::io
