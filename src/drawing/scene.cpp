#include "drawing/scene.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace paint::drawing {

void Scene::add(std::shared_ptr<SceneObject> object) {
    _objects.push_back(std::move(object));
}

void Scene::insert(std::size_t index, std::shared_ptr<SceneObject> object) {
    if (index > _objects.size()) {
        throw std::out_of_range("Index out of range in Scene::insert");
    }

    _objects.insert(_objects.begin() + static_cast<std::ptrdiff_t>(index), std::move(object));
}

void Scene::remove(const std::shared_ptr<SceneObject>& object) {
    auto it = std::find(_objects.begin(), _objects.end(), object);

    if (it == _objects.end()) {
        throw std::out_of_range("SceneObject not found in Scene::remove");
    }

    _objects.erase(it);
}

void Scene::clear() {
    _objects.clear();
}

std::size_t Scene::indexOf(const std::shared_ptr<SceneObject>& object) const {
    auto it = std::find(_objects.begin(), _objects.end(), object);

    if (it != _objects.end()) {
        return std::distance(_objects.begin(), it);
    }

    throw std::out_of_range("SceneObject not found in Scene::indexOf");
}

void Scene::setObjects(const std::vector<std::shared_ptr<SceneObject>>& objects) {
    _objects = objects;
}

}  // namespace paint::drawing
