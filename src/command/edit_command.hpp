#pragma once

#include "command/command.hpp"
#include "drawing/scene.hpp"

using paint::drawing::Scene;
using paint::drawing::SceneObject;

namespace paint {

class EditCommand : public IUndoableCommand {
   public:
    EditCommand(Scene& scene) : _scene(scene) {}

   protected:
    Scene& _scene;
};

class AddObjectCommand : public EditCommand {
   public:
    AddObjectCommand(Scene& scene, std::shared_ptr<SceneObject> object)
        : EditCommand(scene), _object(std::move(object)) {}

    void execute() override { _scene.add(_object); }

    void undo() override { _scene.remove(_object); }

   private:
    std::shared_ptr<SceneObject> _object;
};

class InsertObjectCommand : public EditCommand {
   public:
    InsertObjectCommand(Scene& scene, std::size_t index, std::shared_ptr<SceneObject> object)
        : EditCommand(scene), _index(index), _object(std::move(object)) {}

    void execute() override { _scene.insert(_index, _object); }

    void undo() override { _scene.remove(_object); }

   private:
    std::size_t _index;
    std::shared_ptr<SceneObject> _object;
};

class RemoveObjectCommand : public EditCommand {
   public:
    RemoveObjectCommand(Scene& scene, std::shared_ptr<SceneObject> object)
        : EditCommand(scene), _object(std::move(object)) {}

    void execute() override {
        _index = _scene.indexOf(_object);
        _scene.remove(_object);
    }

    void undo() override { _scene.insert(_index, _object); }

   private:
    std::size_t _index = 0;
    std::shared_ptr<SceneObject> _object;
};

class ClearSceneCommand : public EditCommand {
   public:
    ClearSceneCommand(Scene& scene) : EditCommand(scene), _backupShapes(scene.objects()) {}

    void execute() override { _scene.clear(); }

    void undo() override { _scene.setObjects(_backupShapes); }

   private:
    std::vector<std::shared_ptr<SceneObject>> _backupShapes;
};

}  // namespace paint
