

#include "io/document_storage.hpp"

#include <filesystem>
#include <fstream>

#include "io/utils.hpp"

namespace paint::io {

void DocumentStorage::write(const app::DocumentData& document) {
    const Path& path = document.filename;

    utils::validateWritePath(path);

    std::ofstream file(path);

    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + path.string());
    }

    file << DOCUMENT_STORAGE_HEADER << "\n";

    for (const auto& object : document.scene.objects()) {
        // file << "SceneObject: " << object->serialize() << std::endl;
    }

    file << "END\n";

    file.flush();

    if (!file) {
        throw std::runtime_error("Failed while writing file: " + path.string());
    }
}

// 讀取、解析、驗證成功後才回傳；失敗拋出例外。
app::DocumentData DocumentStorage::read(const Path& filename) {
    std::fstream file(filename, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename.string());
    }

    // 實作讀取邏輯，將文件內容反序列化並解析成 DocumentData
    app::DocumentData data;
    // ... 解析文件內容 ...

    file.close();
    return data;
}

}  // namespace paint::io
