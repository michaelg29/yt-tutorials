#include "keyboard.h"

bool Keyboard::keys[GLFW_KEY_LAST] = { 0 };

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keys[key] = action == GLFW_PRESS;
}

bool Keyboard::key(int key) {
	return keys[key];
}