#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/scene_object.hpp"
#include "drawing/shape_style.hpp"

namespace paint::drawing {

enum class ShapeKind {
    Point,
    Line,
    Rectangle,
    Ellipse,
    Path,
    Polygon,
};

class ShapeObject : public SceneObject {
   public:
    ShapeObject(ShapeStyle style) : _style(style) {}

    virtual ~ShapeObject() = default;

    ObjectKind objectKind() const override { return ObjectKind::Shape; }
    virtual ShapeKind shapeKind() const = 0;

    const ShapeStyle& style() const { return _style; }   // 取得形狀的樣式資訊
    virtual bool isClosed() const = 0;                   // 是否為封閉形狀
    virtual std::vector<Point> getVertices() const = 0;  // 採樣取得形狀的頂點座標，供繪製邊框使用

   protected:
    const ShapeStyle _style;
};

class PointShape : public ShapeObject {
   public:
    PointShape(const ShapeStyle& style) : ShapeObject(style) {}

    bool isClosed() const override { return false; }
    ShapeKind shapeKind() const override { return ShapeKind::Point; }

    std::vector<Point> getVertices() const override;

    void setPosition(const Point& p) { position = p; }
    const Point& getPosition() const { return position; }

   private:
    Point position;
};

// 可以由兩點（起點與終點）定義的形狀，例如直線、矩形、橢圓等。
class TwoPointShape : public ShapeObject {
   public:
    using ShapeObject::ShapeObject;

    void setStart(const Point& point) { _start = point; }
    const Point& start() const { return _start; }

    void setEnd(const Point& point) { _end = point; }
    const Point& end() const { return _end; }

   protected:
    Point _start, _end;
};

class LineShape : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return false; }
    ShapeKind shapeKind() const override { return ShapeKind::Line; }

    std::vector<Point> getVertices() const override;
};

class RectangleShape : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return true; }
    ShapeKind shapeKind() const override { return ShapeKind::Rectangle; }

    std::vector<Point> getVertices() const override;
};

class EllipseShape : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return true; }
    ShapeKind shapeKind() const override { return ShapeKind::Ellipse; }

    std::vector<Point> getVertices() const override;
};

class PathShape : public ShapeObject {
   public:
    using ShapeObject::ShapeObject;

    bool isClosed() const override { return false; }
    ShapeKind shapeKind() const override { return ShapeKind::Path; }

    std::vector<Point> getVertices() const override;

    void addPoint(const Point& p, const bool force = false);

   private:
    std::vector<Point> points;
};

class PolygonShape : public ShapeObject {
   public:
    using ShapeObject::ShapeObject;

    bool isClosed() const override { return points.size() >= 3; }
    ShapeKind shapeKind() const override { return ShapeKind::Polygon; }

    std::vector<Point> getVertices() const override;

    std::size_t pointCount() const { return points.size(); }

    void addPoint(const Point& point);
    void setLastPoint(const Point& point);
    void removeLastPoint();

   private:
    std::vector<Point> points;
};

}  // namespace paint::drawing
