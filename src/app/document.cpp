#include "app/document.hpp"

#include <stdexcept>
#include <utility>

#include "command/edit_command.hpp"
#include "command/file_command.hpp"

namespace paint::app {

DocumentData Document::data() const {
    DocumentData data{_filename, _scene, _canvasWidth, _canvasHeight};
    return data;
}

void Document::setCanvasSize(int width, int height) {
    if (width < 0 || height < 0) {
        throw std::invalid_argument("Canvas dimensions cannot be negative");
    }
    _canvasWidth = width;
    _canvasHeight = height;
}

void Document::replaceContent(Scene scene, Path filename) {
    _scene = std::move(scene);
    _filename = std::move(filename);
    _history.reset();
}

// 文件操作

void Document::newFile() {
    NewFileCommand newFileCommand(*this);
    newFileCommand.execute();
}

void Document::load(const Path& filename) {
    LoadCommand loadCommand(*this, filename);
    loadCommand.execute();
}

void Document::save() {
    if (_filename.empty()) {
        // 如果當前文件名為空，則需要提示用戶輸入文件名
        // 這裡可以彈出一個對話框讓用戶輸入文件名，或者使用默認文件名
        // 例如：
        Path defaultFilename = DEFAULT_FILEPATH;
        save(defaultFilename);
    } else {
        save(_filename);
    }
}

void Document::save(const Path& filename) {
    // 實現保存文件的邏輯
    SaveCommand saveCommand(*this, filename);
    saveCommand.execute();
}

void Document::exportImage(const Path& filename) {
    // 實現導出圖像的邏輯
    ExportCommand exportCommand(*this, filename);
    exportCommand.execute();
}

// 編輯操作

void Document::addShape(std::shared_ptr<Shape> shape) {
    _history.execute(std::make_unique<AddShapeCommand>(_scene, std::move(shape)));
}

void Document::insertShape(std::size_t index, std::shared_ptr<Shape> shape) {
    _history.execute(std::make_unique<InsertShapeCommand>(_scene, index, std::move(shape)));
}

void Document::removeShape(std::shared_ptr<Shape> shape) {
    _history.execute(std::make_unique<RemoveShapeCommand>(_scene, std::move(shape)));
}

void Document::clearScene() {
    if (_scene.size() == 0) {
        return;  // 如果場景已經是空的，則不執行清除操作
    }

    _history.execute(std::make_unique<ClearSceneCommand>(_scene));
}

void Document::undo() {
    _history.undo();
}

void Document::redo() {
    _history.redo();
}

}  // namespace paint::app
