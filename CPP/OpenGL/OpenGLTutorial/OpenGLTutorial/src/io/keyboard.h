#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

class Keyboard {
public:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static bool key(int key);

private:
	static bool keys[];
};

#endif