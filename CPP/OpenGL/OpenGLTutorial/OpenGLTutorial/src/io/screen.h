#ifndef SCREEN_H
#define SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Screen {
public:
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Screen();

	bool init();

	void setParameters();

	// main loop
	void update();
	void newFrame();

	// window closing
	bool shouldClose();
	void setShouldClose(bool shouldClose);

private:
	GLFWwindow* window;
};

#endif