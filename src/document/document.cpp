#include "document/document.hpp"

#include "command/edit_command.hpp"

namespace paint {

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
    _history.execute(std::make_unique<ClearSceneCommand>(_scene));
}

void Document::undo() {
    _history.undo();
}

void Document::redo() {
    _history.redo();
}

}  // namespace paint
