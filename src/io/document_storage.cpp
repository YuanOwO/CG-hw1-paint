

#include "io/document_storage.hpp"

#include <filesystem>
#include <fstream>

namespace paint {

void DocumentStorage::write(const DocumentData& document) {
    const Path& filename = document._filename;

    if (filename.empty()) {
        throw std::runtime_error("Cannot write: file path is empty");
    }

    if (std::filesystem::exists(filename)) {
        if (std::filesystem::is_directory(filename)) {
            throw std::runtime_error("Cannot write to a directory: " + filename.string());
        } else if (!std::filesystem::is_regular_file(filename)) {
            throw std::runtime_error("Cannot write to a non-regular file: " + filename.string());
        }

        // TODO: 文檔重複，提示使用者是否覆蓋
    }

    std::fstream file(filename, std::ios::out);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename.string());
    }

    // 實作寫入邏輯，將 document 的內容序列化並寫入文件
    file << "YUAN_PAINT 1" << std::endl;
    file << "START" << std::endl;
    file << "Hello World!" << std::endl;

    file << "Scene Size: " << document._scene.size() << std::endl;

    for (auto& shape : document._scene.getShapes()) {
        // 假設 Shape 有一個方法可以序列化自己
        // file << "Shape: " << shape->serialize() << std::endl;
    }

    file << "END" << std::endl;

    file.close();
}

// 讀取、解析、驗證成功後才回傳；失敗拋出例外。
DocumentData DocumentStorage::read(const Path& filename) {
    std::fstream file(filename, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename.string());
    }

    // 實作讀取邏輯，將文件內容反序列化並解析成 DocumentData
    DocumentData data;
    // ... 解析文件內容 ...

    file.close();
    return data;
}

}  // namespace paint
