#pragma once

#include <GL/freeglut.h>

#include <vector>

#include "types.hpp"

namespace shape {

class Shape {
   public:
    Shape(GLfloat w, const GLfloat* c, const GLfloat* fc)
        : width(w), color{c[0], c[1], c[2], c[3]}, fillColor{fc[0], fc[1], fc[2], fc[3]} {}

    virtual ~Shape() = default;
    virtual void draw() const = 0;

   protected:
    const GLfloat width;
    const GLfloat color[4];
    const GLfloat fillColor[4];
};

class Line : public Shape {
   public:
    Line(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    void setStart(const Point& point) { start = point; }
    const Point& getStart() const { return start; }
    void setEnd(const Point& point) { end = point; }
    const Point& getEnd() const { return end; }

   private:
    Point start, end;
};

class Stroke : public Shape {
   public:
    Stroke(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    void addPoint(const Point& p) { points.push_back(p); }

   private:
    std::vector<Point> points;
};

class Rectangle : public Shape {
   public:
    Rectangle(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    void setStart(const Point& point) { start = point; }
    const Point& getStart() const { return start; }
    void setEnd(const Point& point) { end = point; }
    const Point& getEnd() const { return end; }

   private:
    Point start, end;
};

class Ellipse : public Shape {
   public:
    Ellipse(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    void setStart(const Point& point) { start = point; }
    const Point& getStart() const { return start; }
    void setEnd(const Point& point) { end = point; }
    const Point& getEnd() const { return end; }

   private:
    Point start, end;
};

class Polygon : public Shape {
   public:
    Polygon(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    std::size_t pointCount() const { return points.size(); }

    void addPoint(const Point& point) { points.push_back(point); }
    void setLastPoint(const Point& point) {
        if (!points.empty()) points.back() = point;
    }
    void removeLastPoint() {
        if (!points.empty()) points.pop_back();
    }

   private:
    std::vector<Point> points;
};

}  // namespace shape
