#pragma once

#include <memory>
#include <vector>

#include "common/color.hpp"
#include "drawing/shapeStyle.hpp"

namespace paint::drawing {

enum class Tool {
    TOOL_PENCIL,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_ELLIPSE,
    TOOL_POLYGON,
};

void setTool(Tool tool);
Tool getTool();

void setLineWidth(int width);
int getLineWidth();

void setColor(const ColorRGBA& color);
ColorRGBA getColor();

void setFillColor(const ColorRGBA& color);
ColorRGBA getFillColor();

void setLineJoin(LineJoin join);
LineJoin getLineJoin();

void setLineCap(LineCap cap);
LineCap getLineCap();

void clearCanvas();

void init();

void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passiveMotion(int x, int y);

void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);

void display();
void reshape(int width, int height);

}  // namespace paint::drawing
