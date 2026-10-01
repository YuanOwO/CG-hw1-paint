#pragma once

#include <string>

#include "app/document.hpp"

using paint::app::Document;
using paint::app::DocumentData;

namespace paint {

// class SvgExporter {
//    public:
//     // 寫入失敗拋出例外。
//     static void write(const DocumentData& document);
// };

class PpmExporter {
   public:
    // 寫入失敗拋出例外。
    static void write(const DocumentData& document);
};

}  // namespace paint
