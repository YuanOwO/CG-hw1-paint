#pragma once

#include <string>

#include "app/document.hpp"

namespace paint::io {

// class SvgExporter {
//    public:
//     // 寫入失敗拋出例外。
//     static void write(const DocumentData& document);
// };

class PpmExporter {
   public:
    // 寫入失敗拋出例外。
    static void write(const app::DocumentData& document);
};

}  // namespace paint::io
