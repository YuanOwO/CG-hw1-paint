#include <GL/freeglut.h>

#include <cstdlib>
#include <iostream>

#include "confirm.hpp"
#include "draw.hpp"
#include "menu.hpp"

namespace {

const int DEFAULT_WINDOW_WIDTH = 640;
const int DEFAULT_WINDOW_HEIGHT = 480;
int idleCounter = 0;

void resetIdleCounter() {
    idleCounter = 0;
}

// GLUT Global callback functions

void idle() {
    // Handle idle events if needed
    idleCounter++;
}

// GLUT Window-specific callback functions

void keyboard(unsigned char key, int x, int y) {
    if (confirm::isOpen()) return;

    // Handle keyboard events if needed
    draw::keyDown(key, x, y);
}

void keyboardUp(unsigned char key, int x, int y) {
    if (confirm::isOpen()) return;

    // Handle key release events if needed
    draw::keyUp(key, x, y);
}

void special(int key, int x, int y) {
    if (confirm::isOpen()) return;

    // Handle special keys if needed
    draw::specialKeyDown(key, x, y);
}

void specialUp(int key, int x, int y) {
    if (confirm::isOpen()) return;

    // Handle special key release events if needed
    draw::specialKeyUp(key, x, y);
}

void mouse(int button, int state, int x, int y) {
    if (confirm::isOpen()) return;

    // Handle mouse events if needed
    draw::mouse(button, state, x, y);
}

void motion(int x, int y) {
    if (confirm::isOpen()) return;

    // Handle mouse motion events if needed
    draw::motion(x, y);
}

void passiveMotion(int x, int y) {
    if (confirm::isOpen()) return;

    // Handle passive mouse motion events if needed
    draw::passiveMotion(x, y);
}

void entry(int state) {
    if (confirm::isOpen()) return;

    // Handle window entry/exit events if needed
}

void reshape(int width, int height) {
    if (confirm::isOpen()) return;

    // Handle window resizing if needed
    draw::reshape(width, height);
}

void visible(int state) {
    if (confirm::isOpen()) return;

    if (state == GLUT_VISIBLE) {
        // Handle window becoming visible
    } else {
        // Handle window becoming invisible
    }
}

void display() {
    if (confirm::isOpen()) return;

    resetIdleCounter();

    // Handle rendering here
    draw::display();
}

}  // namespace

int main(int argc, char** argv) {
    // Initialize GLUT and create the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glutInitWindowPosition(1000, 200);
    glutCreateWindow("OpenGL Painter");

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    // Set up associated callback functions
    glutIdleFunc(idle);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    glutEntryFunc(entry);

    glutReshapeFunc(reshape);
    glutVisibilityFunc(visible);
    glutDisplayFunc(display);

    // Other initialization code can go here
    draw::init();
    menu::init();

    // Enter the GLUT main loop to start processing events
    glutMainLoop();

    return 0;
}
