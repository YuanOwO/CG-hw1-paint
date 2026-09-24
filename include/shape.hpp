#pragma once

#include <GL/freeglut.h>

#include <vector>

#include "types.hpp"

namespace shape {

class Shape {
   public:
    Shape(GLfloat _width, const GLfloat* _color, const GLfloat* _fillColor)
        : width(_width),
          color{_color[0], _color[1], _color[2], _color[3]},
          fillColor{_fillColor[0], _fillColor[1], _fillColor[2], _fillColor[3]} {}

    virtual ~Shape() = default;

    virtual void draw() const {
        auto vertices = getVertices();
        fillShape(vertices);
        drawBorder(vertices);
    }

   protected:
    const GLfloat width;
    const GLfloat color[4];
    const GLfloat fillColor[4];

    virtual bool isClosed() const = 0;           // 是否封閉形狀（例如多邊形、矩形、圓形等）
    virtual bool isRoundedVertices() const = 0;  // 是否在頂點處繪製圓點

    virtual std::vector<Point> getVertices() const = 0;  // 採樣取得形狀的頂點座標，供繪製邊框使用
    virtual void fillShape(const std::vector<Point>& vertices) const;
    virtual void drawBorder(const std::vector<Point>& vertices) const;
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

    // void draw() override;

   protected:
    bool isClosed() const override { return false; }
    bool isRoundedVertices() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Rectangle : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    // void draw() override;

   protected:
    bool isClosed() const override { return true; }
    bool isRoundedVertices() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Ellipse : public TwoPointShape {
   public:
    using TwoPointShape::TwoPointShape;

    // void draw() override;

   protected:
    bool isClosed() const override { return true; }
    bool isRoundedVertices() const override { return true; }

    std::vector<Point> getVertices() const override;
};

class Stroke : public Shape {
   public:
    using Shape::Shape;

    // void draw() override;

    void addPoint(const Point& p) { points.push_back(p); }

   protected:
    bool isClosed() const override { return false; }
    bool isRoundedVertices() const override { return true; }

    std::vector<Point> getVertices() const override;

   private:
    std::vector<Point> points;
};

class Polygon : public Shape {
   public:
    using Shape::Shape;

    // void draw() override;

    std::size_t pointCount() const { return points.size(); }

    void addPoint(const Point& point) { points.push_back(point); }
    void setLastPoint(const Point& point) {
        if (!points.empty()) points.back() = point;
    }
    void removeLastPoint() {
        if (!points.empty()) points.pop_back();
    }

   protected:
    bool isClosed() const override { return true; }
    bool isRoundedVertices() const override { return true; }

    std::vector<Point> getVertices() const override;

   private:
    std::vector<Point> points;
};

}  // namespace shape
