#pragma once

#include <string>

#include "document/document.hpp"

namespace paint {

class SvgExporter {
   public:
    // 寫入失敗拋出例外。
    static void write(const DocumentData& document);
};

class PpmExporter {
   public:
    // 寫入失敗拋出例外。
    static void write(const DocumentData& document);
};

}  // namespace paint
