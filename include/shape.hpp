#pragma once

#include <GL/freeglut.h>

#include <vector>

#include "types.hpp"

namespace shape {

enum class ShapeEventResult {
    NONE,    // 不需要提交草稿
    COMMIT,  // 提交草稿
    CANCEL,  // 取消草稿
};

class Shape {
   public:
    Shape(GLfloat w, const GLfloat* c, const GLfloat* fc)
        : width(w), color{c[0], c[1], c[2], c[3]}, fillColor{fc[0], fc[1], fc[2], fc[3]} {}

    virtual ~Shape() = default;
    virtual void draw() const = 0;

    virtual ShapeEventResult onMouseDown(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onMouseUp(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onMouseMove(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onMousePassiveMove(EventState& eventState) { return ShapeEventResult::NONE; }

    virtual ShapeEventResult onKeyDown(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onKeyUp(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onSpecialKeyDown(EventState& eventState) { return ShapeEventResult::NONE; }
    virtual ShapeEventResult onSpecialKeyUp(EventState& eventState) { return ShapeEventResult::NONE; }

   protected:
    const GLfloat width;
    const GLfloat color[4];
    const GLfloat fillColor[4];
};

class Line : public Shape {
   public:
    Line(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    ShapeEventResult onMouseDown(EventState& eventState) override;
    ShapeEventResult onMouseUp(EventState& eventState) override;
    ShapeEventResult onMouseMove(EventState& eventState) override;

   private:
    Point start, end;
};

class Stroke : public Shape {
   public:
    Stroke(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    ShapeEventResult onMouseDown(EventState& eventState) override;
    ShapeEventResult onMouseUp(EventState& eventState) override;
    ShapeEventResult onMouseMove(EventState& eventState) override;

    void addPoint(const Point& p) { points.push_back(p); }

   private:
    std::vector<Point> points;
};

class Rectangle : public Shape {
   public:
    Rectangle(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    ShapeEventResult onMouseDown(EventState& eventState) override;
    ShapeEventResult onMouseUp(EventState& eventState) override;
    ShapeEventResult onMouseMove(EventState& eventState) override;

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

class Polygon : public Shape {
   public:
    Polygon(GLfloat w, const GLfloat* c, const GLfloat* fc) : Shape(w, c, fc) {}

    void draw() const override;

    ShapeEventResult onMouseDown(EventState& eventState) override;
    ShapeEventResult onMouseUp(EventState& eventState) override;
    ShapeEventResult onMouseMove(EventState& eventState) override;
    ShapeEventResult onMousePassiveMove(EventState& eventState) override;
    ShapeEventResult onKeyDown(EventState& eventState) override;

   private:
    std::vector<Point> points;
};

}  // namespace shape
