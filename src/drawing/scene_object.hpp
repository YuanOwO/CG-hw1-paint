#pragma once

namespace paint::drawing {

enum class ObjectKind { Shape, Text };

class SceneObject {
   public:
    virtual ~SceneObject() = default;

    virtual ObjectKind objectKind() const = 0;
};

}  // namespace paint::drawing
