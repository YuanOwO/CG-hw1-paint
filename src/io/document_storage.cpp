#include "io/document_storage.hpp"

#include <fstream>
#include <stdexcept>

#include "io/utils.hpp"

namespace paint::io {

void DocumentStorage::write(const app::DocumentData& data, const Serializer& serializer) {
    const Path& path = data.filename;

    utils::validateWritePath(path);

    // Serializer 可能會用 byte 數保存文字或其他資料，因此一律用 binary mode。
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + path.string());
    }

    serializer.serialize(file, data);

    file.flush();

    if (!file) {
        throw std::runtime_error("Failed while writing file: " + path.string());
    }

    file.close();

    if (!file) {
        throw std::runtime_error("Failed while closing file: " + path.string());
    }
}

app::DocumentData DocumentStorage::read(const Path& filename, const Serializer& serializer) {
    utils::validateReadPath(filename);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + filename.string());
    }

    app::DocumentData data = serializer.deserialize(file);

    // 檔名是 Storage 層的資訊，不是文件格式的一部分。
    data.filename = filename;
    return data;
}

}  // namespace paint::io
