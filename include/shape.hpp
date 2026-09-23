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

    virtual bool onMouseDown(EventState& eventState) { return false; }
    virtual bool onMouseUp(EventState& eventState) { return false; }
    virtual bool onMouseMove(EventState& eventState) { return false; }

    virtual bool onKeyDown(EventState& eventState) { return false; }
    virtual bool onKeyUp(EventState& eventState) { return false; }
    virtual bool onSpecialKeyDown(EventState& eventState) { return false; }
    virtual bool onSpecialKeyUp(EventState& eventState) { return false; }

   protected:
    const GLfloat width;
    const GLfloat color[4];
    const GLfloat fillColor[4];
};

class Line : public Shape {
   public:
    Line(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    bool onMouseDown(EventState& eventState) override;
    bool onMouseUp(EventState& eventState) override;
    bool onMouseMove(EventState& eventState) override;

   private:
    Point start, end;
};

class Stroke : public Shape {
   public:
    Stroke(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    bool onMouseDown(EventState& eventState) override;
    bool onMouseUp(EventState& eventState) override;
    bool onMouseMove(EventState& eventState) override;

    void addPoint(const Point& p) { points.push_back(p); }

   private:
    std::vector<Point> points;
};

class Rectangle : public Shape {
   public:
    Rectangle(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    bool onMouseDown(EventState& eventState) override;
    bool onMouseUp(EventState& eventState) override;
    bool onMouseMove(EventState& eventState) override;

   private:
    Point start, end;

    void updateEdges();
};

// class Ellipse : public Shape {
//    public:
//     void draw() const override;

//     void setStart(const Point& s) { start = s; }
//     Point getStart() const { return start; }

//     void setEnd(const Point& e) { end = e; }
//     Point getEnd() const { return end; }

//    private:
//     Point start, end;
// };

// class Polygon : public Shape {
//    public:
//     void draw() const override;

//     void addPoint(const Point& p) { points.push_back(p); }

//    private:
//     std::vector<Point> points;
// };

}  // namespace shape
