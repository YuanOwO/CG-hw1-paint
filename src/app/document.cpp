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
    _canvasWidth = 0;
    _canvasHeight = 0;
    _history.reset();
    markSaved();  // 替換內容後，標記為未修改
}

void Document::replaceContent(DocumentData data) {
    _scene = std::move(data.scene);
    _filename = std::move(data.filename);
    _canvasWidth = data.canvasWidth;
    _canvasHeight = data.canvasHeight;
    _history.reset();
    markSaved();  // 替換內容後，標記為未修改
}

// 文件操作

void Document::newFile() {
    NewFileCommand newFileCommand(*this);
    newFileCommand.execute();
    onModifiedChangedInternal();
}

void Document::load(const Path& filename) {
    LoadCommand loadCommand(*this, filename);
    loadCommand.execute();
    onModifiedChangedInternal();
}

void Document::save() {
    save(_filename);
}

void Document::save(const Path& filename) {
    // 實現保存文件的邏輯
    SaveCommand saveCommand(*this, filename);
    saveCommand.execute();
    onModifiedChangedInternal();
}

void Document::exportImage(const Path& filename) {
    // 實現導出圖像的邏輯
    ExportCommand exportCommand(*this, filename);
    exportCommand.execute();
    onModifiedChangedInternal();
}

// 編輯操作

void Document::addObject(std::shared_ptr<SceneObject> object) {
    _history.execute(std::make_unique<AddObjectCommand>(_scene, std::move(object)));
    onModifiedChangedInternal();
}

void Document::insertObject(std::size_t index, std::shared_ptr<SceneObject> object) {
    _history.execute(std::make_unique<InsertObjectCommand>(_scene, index, std::move(object)));
    onModifiedChangedInternal();
}

void Document::removeObject(std::shared_ptr<SceneObject> object) {
    _history.execute(std::make_unique<RemoveObjectCommand>(_scene, std::move(object)));
    onModifiedChangedInternal();
}

void Document::clearScene() {
    if (_scene.size() == 0) {
        return;  // 如果場景已經是空的，則不執行清除操作
    }

    _history.execute(std::make_unique<ClearSceneCommand>(_scene));
    onModifiedChangedInternal();
}

void Document::undo() {
    _history.undo();
    onModifiedChangedInternal();
}

void Document::redo() {
    _history.redo();
    onModifiedChangedInternal();
}

}  // namespace paint::app
