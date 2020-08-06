#include "mouse.h"
#include "camera.h"

/*
    define initial static values
*/

// x posiiton
double Mouse::x = 0;
// y position
double Mouse::y = 0;

// previous x position
double Mouse::lastX = 0;
// previous y position
double Mouse::lastY = 0;

// change in x position from lastX
double Mouse::dx = 0;
// change in y position from lastY
double Mouse::dy = 0;

// change in scroll x
double Mouse::scrollDx = 0;
// change in scroll y
double Mouse::scrollDy = 0;

// if this is the first change in the mouse position
bool Mouse::firstMouse = true;

// button state array (true for down, false for up)
bool Mouse::buttons[GLFW_MOUSE_BUTTON_LAST] = { 0 };
// button changed array (true if changed)
bool Mouse::buttonsChanged[GLFW_MOUSE_BUTTON_LAST] = { 0 };

/*
    callbacks
*/

// cursor position changed
void Mouse::cursorPosCallback(GLFWwindow* window, double _x, double _y) {
    x = _x;
    y = _y;

    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    dx = x - lastX;
    dy = lastY - y; // y coordinates are inverted
    lastX = x;
    lastY = y;
}

// mouse button state changed
void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_RELEASE) {
        if (!buttons[button]) {
            buttons[button] = true;
        }
    }
    else {
        buttons[button] = false;
    }
    buttonsChanged[button] = action != GLFW_REPEAT;
}

// scroll wheel moved
void Mouse::mouseWheelCallback(GLFWwindow* window, double dx, double dy) {
    scrollDx = dx;
    scrollDy = dy;
}

/*
    accessors
*/

// get mouse x position
double Mouse::getMouseX() {
    return x;
}

// get mouse y position
double Mouse::getMouseY() {
    return y;
}

// get mouse change in x
double Mouse::getDX() {
    double _dx = dx;
    // set to 0 because change no longer new
    dx = 0;
    return _dx;
}

// get mouse change in y
double Mouse::getDY() {
    double _dy = dy;
    // set to 0 because change no longer new
    dy = 0;
    return _dy;
}

// get scroll value in x
double Mouse::getScrollDX() {
    double _scrollDx = scrollDx;
    // set to 0 because change no longer new
    scrollDx = 0;
    return _scrollDx;
}

// get scroll value in y
double Mouse::getScrollDY() {
    double _scrollDy = scrollDy;
    // set to 0 because change no longer new
    scrollDy = 0;
    return _scrollDy;
}

// get button state
bool Mouse::button(int button) {
    return buttons[button];
}

// return if button changed then reset it in the changed array
bool Mouse::buttonChanged(int button) {
    bool ret = buttonsChanged[button];
    // set to false because change no longer new
    buttonsChanged[button] = false;
    return ret;
}

// return if button changed and is now up
bool Mouse::buttonWentUp(int button) {
    return !buttons[button] && buttonChanged(button);
}

// return if button changed and is now down
bool Mouse::buttonWentDown(int button) {
    return buttons[button] && buttonChanged(button);
}