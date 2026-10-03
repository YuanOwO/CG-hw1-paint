#include "io/image_exporter.hpp"

#include <filesystem>
#include <fstream>

#include "app/windows/graphicsRenderer.hpp"
#include "io/utils.hpp"

namespace paint {

void PpmExporter::write(const DocumentData& document) {
    if (document.canvasWidth <= 0 || document.canvasHeight <= 0) {
        throw std::runtime_error("Cannot export: canvas size is empty");
    }

    const Path& path = document.filename;

    utils::validateWritePath(path);

    std::ofstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + path.string());
    }

    app::GraphicsRenderWindow renderer(document.canvasWidth, document.canvasHeight, document);

    auto buffer = renderer.capture();

    // 寫入 PPM 標頭
    file << "P6\n" << buffer.width() << " " << buffer.height() << "\n255\n";

    // 寫入像素資料
    const auto& pixels = buffer.pixels();
    for (int i = 0; i < buffer.height(); i++) {
        for (int j = 0; j < buffer.width(); j++) {
            const auto& pixel = pixels[(i * buffer.width() + j) * 4];  // RGBA
            file.write(reinterpret_cast<const char*>(&pixel), 3);      // 寫入 RGB，忽略 A
        }
    }

    file.flush();

    if (!file) {
        throw std::runtime_error("Failed while writing file: " + path.string());
    }
}

}  // namespace paint
