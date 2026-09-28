#pragma once

#include <vector>

#include "common/point.hpp"
#include "drawing/shape_style.hpp"

namespace paint::drawing {

class Shape {
   public:
    Shape(ShapeStyle _style) : style(_style) {}

    virtual ~Shape() = default;

    virtual bool isClosed() const = 0;  // 是否為封閉形狀

    const ShapeStyle& getStyle() const { return style; }

    virtual std::vector<Point> getVertices() const = 0;  // 採樣取得形狀的頂點座標，供繪製邊框使用

   protected:
    const ShapeStyle style;
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

   protected:
    bool isClosed() const override { return false; }

    std::vector<Point> getVertices() const override;
};

class Rectangle : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

   protected:
    bool isClosed() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Ellipse : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

   protected:
    bool isClosed() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Path : public Shape {
   public:
    using Shape::Shape;

    void addPoint(const Point& p, const bool force = false) {
        if (points.empty()) {
            points.push_back(p);
            return;
        }

        // 避免筆刷的點太密集，導致繪製出來的線條過於粗糙。
        const bool isTooClose = abs(points.back() - p) < std::max(style.stroke.width * 0.2f, 1.0f);

        if (force && isTooClose && points.size() >= 2) {
            // 強制加入點時，若太接近前一個點，則將前一個點移除，避免重疊。
            points.pop_back();
        }

        if (force || !isTooClose) {
            points.push_back(p);
        }
    }

   protected:
    bool isClosed() const override { return false; }

    std::vector<Point> getVertices() const override;

   private:
    std::vector<Point> points;
};

class Polygon : public Shape {
   public:
    using Shape::Shape;

    std::size_t pointCount() const { return points.size(); }

    void addPoint(const Point& point) { points.push_back(point); }
    void setLastPoint(const Point& point) {
        if (!points.empty()) points.back() = point;
    }
    void removeLastPoint() {
        if (!points.empty()) points.pop_back();
    }

   protected:
    bool isClosed() const override { return points.size() >= 3; }

    std::vector<Point> getVertices() const override;

   private:
    std::vector<Point> points;
};

}  // namespace paint::drawing
