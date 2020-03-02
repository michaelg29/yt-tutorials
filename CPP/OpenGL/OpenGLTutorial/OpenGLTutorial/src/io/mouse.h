#ifndef MOUSE_H
#define MOUSE_H

#include <GLFW/glfw3.h>

class Mouse {
public:
	static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static double getMouseX();
	static double getMouseY();

	static bool button(int _button);

private:
	static double x;
	static double y;

	static bool buttons[];
};

#endif