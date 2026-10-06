#pragma once

#include "app/document.hpp"
#include "io/serializer/serializer.hpp"

namespace paint::io {

class DocumentStorage {
   public:
    // 寫入失敗拋出例外。
    static void write(const app::DocumentData& document, const Serializer& serializer);

    // 讀取、解析、驗證成功後才回傳；失敗拋出例外。
    static app::DocumentData read(const Path& filename, const Serializer& serializer);
};

}  // namespace paint::io
