#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "command/command_history.hpp"
#include "drawing/scene.hpp"
#include "drawing/shape.hpp"

using paint::drawing::Scene;
using paint::drawing::Shape;

namespace paint {

class Document {
   public:
    Document() = default;
    Document(const std::string& filename) : _filename(filename) {}

    // 禁止拷貝和賦值
    Document(const Document& other) = delete;
    Document& operator=(const Document& other) = delete;

    const Scene& getScene() { return _scene; }

    std::string getFilename() const { return _filename; }
    void setFilename(const std::string& filename) { _filename = filename; }

    void addShape(std::shared_ptr<Shape> shape);
    void insertShape(std::size_t index, std::shared_ptr<Shape> shape);
    void removeShape(std::shared_ptr<Shape> shape);
    void clearScene();

    void undo();
    void redo();

    bool canUndo() const { return _history.canUndo(); }
    bool canRedo() const { return _history.canRedo(); }

    // void saveToFile();

   private:
    std::string _filename;
    CommandHistory _history;
    Scene _scene;
};

}  // namespace paint
