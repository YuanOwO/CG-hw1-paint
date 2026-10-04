#pragma once

#include <string>

#include "app/application.hpp"
#include "app/document.hpp"

namespace paint::io {

static const std::string DOCUMENT_STORAGE_SERIALIZER_VERSION = "1";  // 文件格式版本號

static std::string getDocumentStorageHeader() {
    std::string header = app::Application::name();
    for (auto&& c : header) {
        c = std::toupper(c);
        if (c == ' ') {
            c = '_';
        }
    }
    return header + " " + DOCUMENT_STORAGE_SERIALIZER_VERSION;
}

static const std::string DOCUMENT_STORAGE_HEADER = getDocumentStorageHeader();

class DocumentStorage {
   public:
    // 寫入失敗拋出例外。
    static void write(const app::DocumentData& document);

    // 讀取、解析、驗證成功後才回傳；失敗拋出例外。
    static app::DocumentData read(const Path& filename);
};

}  // namespace paint::io
