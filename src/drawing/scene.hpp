#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "drawing/scene_object.hpp"

namespace paint::drawing {

class Scene {
   public:
    void add(std::shared_ptr<SceneObject> object);
    void insert(std::size_t index, std::shared_ptr<SceneObject> object);
    void remove(const std::shared_ptr<SceneObject>& object);
    void clear();

    std::size_t indexOf(const std::shared_ptr<SceneObject>& object) const;
    std::size_t size() const { return _objects.size(); }

    const std::vector<std::shared_ptr<SceneObject>>& objects() const { return _objects; }
    void setObjects(const std::vector<std::shared_ptr<SceneObject>>& objects);

   private:
    std::vector<std::shared_ptr<SceneObject>> _objects;
};

}  // namespace paint::drawing
