#include "joystick.h"

/*
    constructor
*/

// generate an instance for joystick with id i
Joystick::Joystick(int i) {
    id = getId(i);

    update();
}

// update the joystick's states
void Joystick::update() {
    present = glfwJoystickPresent(id);

    if (present) {
        name = glfwGetJoystickName(id);
        axes = glfwGetJoystickAxes(id, &axesCount);
        buttons = glfwGetJoystickButtons(id, &buttonCount);
    }
}

/*
    accessors
*/

// get axis value
float Joystick::axesState(int axis) {
    if (present) {
        return axes[axis];
    }

    return -1;
}

// get button state
unsigned char Joystick::buttonState(int button) {
    if (present) {
        return buttons[button];
    }

    return GLFW_RELEASE;
}

// get number of axes
int Joystick::getAxesCount() {
    return axesCount;
}

// get number of buttons
int Joystick::getButtonCount() {
    return buttonCount;
}

// return if joystick present
bool Joystick::isPresent() {
    return present;
}

// get name of joystick
const char* Joystick::getName() {
    return name;
}

// static method to get enum value for joystick
int Joystick::getId(int i) {
    return GLFW_JOYSTICK_1 + i;
}