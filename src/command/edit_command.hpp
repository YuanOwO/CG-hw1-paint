#pragma once

#include "command/command.hpp"
#include "drawing/scene.hpp"

using paint::drawing::Scene;
using paint::drawing::Shape;

namespace paint {

class EditCommand : public IUndoableCommand {
   public:
    EditCommand(Scene& scene) : _scene(scene) {}

   protected:
    Scene& _scene;
};

class AddShapeCommand : public EditCommand {
   public:
    AddShapeCommand(Scene& scene, std::shared_ptr<Shape> shape)
        : EditCommand(scene), _shape(std::move(shape)) {}

    void execute() override { _scene.add(_shape); }

    void undo() override { _scene.remove(_shape); }

   private:
    std::shared_ptr<Shape> _shape;
};

class InsertShapeCommand : public EditCommand {
   public:
    InsertShapeCommand(Scene& scene, std::size_t index, std::shared_ptr<Shape> shape)
        : EditCommand(scene), _index(index), _shape(std::move(shape)) {}

    void execute() override { _scene.insert(_index, _shape); }

    void undo() override { _scene.remove(_shape); }

   private:
    std::size_t _index;
    std::shared_ptr<Shape> _shape;
};

class RemoveShapeCommand : public EditCommand {
   public:
    RemoveShapeCommand(Scene& scene, std::shared_ptr<Shape> shape)
        : EditCommand(scene), _shape(std::move(shape)) {}

    void execute() override { _scene.remove(_shape); }

    // FIXME: 如果是 remove 中間的 shape，或是 remove 後再 insert，undo 會失敗，因為 shape 的位置資訊會遺失
    void undo() override { _scene.add(_shape); }

   private:
    std::shared_ptr<Shape> _shape;
};

class ClearSceneCommand : public EditCommand {
   public:
    ClearSceneCommand(Scene& scene) : EditCommand(scene) {}

    void execute() override {
        _backupShapes = _scene.getShapes();
        _scene.clear();
    }

    void undo() override { _scene.setShapes(_backupShapes); }

   private:
    std::vector<std::shared_ptr<Shape>> _backupShapes;
};

}  // namespace paint
