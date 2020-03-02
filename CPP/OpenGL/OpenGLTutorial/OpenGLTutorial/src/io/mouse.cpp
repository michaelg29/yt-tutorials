#include "mouse.h"

double Mouse::x = 0;
double Mouse::y = 0;

bool Mouse::buttons[GLFW_MOUSE_BUTTON_LAST] = { 0 };

void Mouse::cursorPosCallback(GLFWwindow* window, double _x, double _y) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// normalize coordinates
	x = (2 * _x / width) - 1;
	y = 2 * (1 - (2 * _y / width));
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	buttons[button] = action == GLFW_PRESS;
}

double Mouse::getMouseX() {
	return x;
}

double Mouse::getMouseY() {
	return y;
}

bool Mouse::button(int _button) {
	return buttons[_button];
}