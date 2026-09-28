#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shape_style.hpp"

namespace paint::drawing {

class Shape {
   public:
    Shape(ShapeStyle _style) : style(_style) {}

    virtual ~Shape() = default;

    const ShapeStyle& getStyle() const { return style; }  // 取得形狀的樣式資訊
    virtual bool isClosed() const = 0;                    // 是否為封閉形狀
    virtual std::vector<Point> getVertices() const = 0;   // 採樣取得形狀的頂點座標，供繪製邊框使用

   protected:
    const ShapeStyle style;
};

class PointShape : public Shape {
   public:
    PointShape(const ShapeStyle& style) : Shape(style) {}

    bool isClosed() const override { return false; }

    std::vector<Point> getVertices() const override;

    void setPosition(const Point& p) { position = p; }
    const Point& getPosition() const { return position; }

   private:
    Point position;
};

// 可以由兩點（起點與終點）定義的形狀，例如直線、矩形、橢圓等。
class TwoPointShape : public Shape {
   public:
    using Shape::Shape;

    void setStart(const Point& point) { start = point; }
    const Point& getStart() const { return start; }

    void setEnd(const Point& point) { end = point; }
    const Point& getEnd() const { return end; }

   protected:
    Point start, end;
};

class Line : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return false; }

    std::vector<Point> getVertices() const override;
};

class Rectangle : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Ellipse : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    bool isClosed() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Path : public Shape {
   public:
    using Shape::Shape;

    bool isClosed() const override { return false; }

    std::vector<Point> getVertices() const override;

    void addPoint(const Point& p, const bool force = false);

   private:
    std::vector<Point> points;
};

class Polygon : public Shape {
   public:
    using Shape::Shape;

    bool isClosed() const override { return points.size() >= 3; }

    std::vector<Point> getVertices() const override;

    std::size_t pointCount() const { return points.size(); }

    void addPoint(const Point& point);
    void setLastPoint(const Point& point);
    void removeLastPoint();

   private:
    std::vector<Point> points;
};

}  // namespace paint::drawing
