#pragma once

#include "drawing/shape_object.hpp"
#include "drawing/tools/creation_tool.hpp"

namespace paint::drawing {

// 直線、矩形、圓形等需要拖曳兩個點的工具可以共用這個基底類別。
template <typename TShape>
class DragTool : public ShapeCreationTool<TShape> {
   public:
    using ShapeCreationTool<TShape>::ShapeCreationTool;

    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override;
    ToolResult onKeyUp(const KeyboardEvent& event, Point localPosition) override;
    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition) override;
    ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition) override;
    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override;

   protected:
    // Shift 鍵被按下時，會畫出正圓、正方形。子類別可以覆寫此方法以實現不同的行為。
    virtual void onShift(const InputEvent& event, Point localPosition);
};

class LineTool : public DragTool<LineShape> {
   public:
    using DragTool<LineShape>::DragTool;

   protected:
    // Shift 鍵被按下時，會畫出水平、垂直或 45° 斜線。
    void onShift(const InputEvent& event, Point localPosition) override;
};

class RectangleTool : public DragTool<RectangleShape> {
   public:
    using DragTool<RectangleShape>::DragTool;
};

class EllipseTool : public DragTool<EllipseShape> {
   public:
    using DragTool<EllipseShape>::DragTool;
};

class PointTool : public ShapeCreationTool<PointShape> {
   public:
    using ShapeCreationTool<PointShape>::ShapeCreationTool;

    ToolResult onClick(const ClickEvent& event, Point localPosition) override;
};

class PencilTool : public ShapeCreationTool<PathShape> {
   public:
    using ShapeCreationTool<PathShape>::ShapeCreationTool;

    ToolResult onMouseDown(const MouseButtonEvent& event, Point localPosition);
    ToolResult onMouseUp(const MouseButtonEvent& event, Point localPosition);
    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition);
};

class PolygonTool : public ShapeCreationTool<PolygonShape> {
   public:
    using ShapeCreationTool<PolygonShape>::ShapeCreationTool;

    ToolResult onKeyDown(const KeyboardEvent& event, Point localPosition) override;
    ToolResult onClick(const ClickEvent& event, Point localPosition) override;
    ToolResult onDoubleClick(const ClickEvent& event, Point localPosition) override;
    ToolResult onMouseMove(const MouseMoveEvent& event, Point localPosition) override;
};

}  // namespace paint::drawing
