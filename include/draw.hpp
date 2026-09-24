#pragma once

#include "color.hpp"

namespace draw {

enum class Tool {
    TOOL_PENCIL,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_ELLIPSE,
    TOOL_POLYGON,
};

void setTool(Tool tool);
Tool getTool();

void setWidth(int width);
int getWidth();

void setColor(const color::ColorRGBA& color);
color::ColorRGBA getColor();

void setFillColor(const color::ColorRGBA& color);
color::ColorRGBA getFillColor();

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

}  // namespace draw
