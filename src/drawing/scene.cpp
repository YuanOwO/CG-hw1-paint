#include "drawing/scene.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace paint::drawing {

void Scene::add(std::shared_ptr<Shape> shape) {
    _shapes.push_back(std::move(shape));
}

void Scene::insert(std::size_t index, std::shared_ptr<Shape> shape) {
    if (index > _shapes.size()) {
        throw std::out_of_range("Index out of range in Scene::insert");
    }

    _shapes.insert(_shapes.begin() + static_cast<std::ptrdiff_t>(index), std::move(shape));
}

void Scene::remove(const std::shared_ptr<Shape>& shape) {
    auto it = std::find(_shapes.begin(), _shapes.end(), shape);

    if (it == _shapes.end()) {
        throw std::out_of_range("Shape not found in Scene::remove");
    }

    _shapes.erase(it);
}

void Scene::clear() {
    _shapes.clear();
}

std::size_t Scene::indexOf(const std::shared_ptr<Shape>& shape) const {
    auto it = std::find(_shapes.begin(), _shapes.end(), shape);

    if (it != _shapes.end()) {
        return std::distance(_shapes.begin(), it);
    }

    throw std::out_of_range("Shape not found in Scene::indexOf");
}

void Scene::setShapes(const std::vector<std::shared_ptr<Shape>>& shapes) {
    _shapes = shapes;
}

}  // namespace paint::drawing
