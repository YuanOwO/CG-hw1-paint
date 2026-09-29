#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "drawing/shape.hpp"

namespace paint::drawing {

class Scene {
   public:
    void add(std::shared_ptr<Shape> shape);
    void insert(std::size_t index, std::shared_ptr<Shape> shape);
    void remove(const std::shared_ptr<Shape>& shape);
    void clear();

    std::size_t indexOf(const std::shared_ptr<Shape>& shape) const;
    std::size_t size() const { return _shapes.size(); }

    const std::vector<std::shared_ptr<Shape>>& shapes() const { return _shapes; }
    void setShapes(const std::vector<std::shared_ptr<Shape>>& shapes);

   private:
    std::vector<std::shared_ptr<Shape>> _shapes;
};

}  // namespace paint::drawing
