#include "drawing/tools/select_tool.hpp"

#include <algorithm>
#include <optional>

#include "common/font.hpp"
#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"

namespace paint::drawing {
namespace {

using SelectionBounds = ToolOverlay::SelectionBounds;

bool sceneContains(const Scene& scene, const std::shared_ptr<SceneObject>& object) {
    return std::find(scene.objects().begin(), scene.objects().end(), object) != scene.objects().end();
}

std::optional<SelectionBounds> objectBounds(const SceneObject& object) {
    if (const auto* shape = dynamic_cast<const ShapeObject*>(&object)) {
        const auto vertices = shape->getVertices();
        if (vertices.empty()) {
            return std::nullopt;
        }

        float left = vertices.front().x();
        float right = left;
        float top = vertices.front().y();
        float bottom = top;

        for (const auto& vertex : vertices) {
            left = std::min(left, vertex.x());
            right = std::max(right, vertex.x());
            top = std::min(top, vertex.y());
            bottom = std::max(bottom, vertex.y());
        }

        // 邊界需包含 stroke，選取框才不會壓在圖形線條上。
        float renderedWidth = shape->style().stroke.width;
        if (const auto* point = dynamic_cast<const PointShape*>(shape)) {
            renderedWidth = point->style().pointSize;
        }
        const float margin = std::max(renderedWidth * 0.5f, 2.0f);
        return SelectionBounds{
            {left - margin,  top - margin   },
            {right + margin, bottom + margin}
        };
    }

    if (const auto* text = dynamic_cast<const TextObject*>(&object)) {
        const float width = getFontWidth(text->style().font, text->text());
        const float height = getFontHeight(text->style().font, text->text());
        return SelectionBounds{text->position(), text->position() + Point(width, height)};
    }

    return std::nullopt;
}

bool hitTest(const SceneObject& object, Point point) {
    if (const auto* text = dynamic_cast<const TextObject*>(&object)) {
        const auto bounds = objectBounds(*text);
        if (!bounds) {
            return false;
        }
        return point.x() >= bounds->topLeft.x() && point.x() <= bounds->bottomRight.x() &&
               point.y() >= bounds->topLeft.y() && point.y() <= bounds->bottomRight.y();
    }

    const auto* shape = dynamic_cast<const ShapeObject*>(&object);
    if (!shape) {
        return false;
    }

    const auto vertices = shape->getVertices();
    if (vertices.empty()) {
        return false;
    }

    // 有填色的封閉圖形可以直接點擊內部；透明圖形則只選得到邊線。
    const bool hasVisibleFill =
        (shape->style().fillMode == FillMode::FILLED && shape->paint().color.a > 0.0f) ||
        (shape->style().fillMode == FillMode::ADVANCED && shape->paint().fillColor.a > 0.0f);
    if (shape->isClosed() && hasVisibleFill && pointInPolygon(point, vertices)) {
        return true;
    }

    if (shape->paint().color.a <= 0.0f) {
        return false;
    }

    float renderedWidth = shape->style().stroke.width;
    if (const auto* pointShape = dynamic_cast<const PointShape*>(shape)) {
        renderedWidth = pointShape->style().pointSize;
    }
    const float tolerance = std::max(renderedWidth * 0.5f + 3.0f, 5.0f);
    if (vertices.size() == 1) {
        return abs(point - vertices.front()) <= tolerance;
    }

    const std::size_t edgeCount = shape->isClosed() ? vertices.size() : vertices.size() - 1;
    for (std::size_t i = 0; i < edgeCount; ++i) {
        if (distanceToSegment(point, vertices[i], vertices[(i + 1) % vertices.size()]) <= tolerance) {
            return true;
        }
    }

    return false;
}

}  // namespace

ToolOverlay SelectTool::overlay() const {
    ToolOverlay result;
    // 如果選取的物件已經不在場景中，則不顯示選取框
    if (_selected && sceneContains(_scene, _selected)) {
        if (const auto bounds = objectBounds(*_selected)) {
            result.selectionBounds.push_back(*bounds);
        }
    }
    return result;
}

ToolResult SelectTool::cancel() {
    if (!_selected) {
        return {};
    }

    _selected.reset();
    return ToolResult::redraw(true);
}

ToolResult SelectTool::onKeyDown(const KeyboardEvent& event, Point localPosition) {
    if (event.key() == Key::Escape) {
        return cancel();
    }

    if (_selected && !sceneContains(_scene, _selected)) {
        _selected.reset();
        return ToolResult::redraw(true);
    }

    if (_selected && (event.key() == Key::Delete || event.key() == Key::Backspace)) {
        auto selected = std::move(_selected);
        return ToolResult::removeObject(std::move(selected), true);
    }

    return {};
}

ToolResult SelectTool::onMouseDown(const MouseButtonEvent& event, Point localPosition) {
    std::shared_ptr<SceneObject> selected;

    // 場景尾端最後繪製，因此反向搜尋可優先選到視覺上的最上層物件。
    for (auto it = _scene.objects().rbegin(); it != _scene.objects().rend(); ++it) {
        if (*it && hitTest(**it, localPosition)) {
            selected = *it;
            break;
        }
    }

    if (selected == _selected) {
        return {true, false, NoAction{}};
    }

    _selected = std::move(selected);
    return ToolResult::redraw(true);
}

}  // namespace paint::drawing
