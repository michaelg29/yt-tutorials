#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float mixVal = 0.5f;

glm::mat4 mouseTransform = glm::mat4(1.0f);

float x, y, z;

Joystick mainJ(0);

int main() {
	int success;
	char infoLog[512];

	std::cout << "Hello, world!" << std::endl;

	glfwInit();

	// openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COPMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial", NULL, NULL);
	if (window == NULL) { // window not created
		std::cout << "Could not create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, Keyboard::keyCallback);

	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);

	// SHADERS===============================
	Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//float vertices[] = {
	//	// positions		// colors			// texture coordinates
	//	-0.5f, -0.5f, 0.0f,	1.0f, 1.0f, 0.5f,	0.0f, 0.0f,	// bottom left
	//	-0.5f, 0.5f, 0.0f,	0.5f, 1.0f, 0.75f,	0.0f, 1.0f,	// top left
	//	0.5f, -0.5f, 0.0f,	0.6f, 1.0f, 0.2f,	1.0f, 0.0f,	// bottom right
	//	0.5f, 0.5f, 0.0f,	1.0f, 0.2f, 1.0f,	1.0f, 1.0f	// top right
	//};
	unsigned int indices[] = {
		0, 1, 2, // first triangle
		3, 1, 2  // second triangle
	};

	// VBO, VAO, EBO
	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &EBO);

	// bind VAO
	glBindVertexArray(VAO);

	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// put index array in EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// set attributes pointers
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	// texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// TEXTURES_____________________________________

	// generate texture
	unsigned int texture1, texture2;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// image wrap (s, t, r) = (x, y, z)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// border color
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // scale up -> blend colors
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// load image 1
	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("assets/image1.jpg", &width, &height, &nChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// load image 2
	data = stbi_load("assets/image2.png", &width, &height, &nChannels, 0);

	if (data) {
		// RGBA because png
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	shader.activate();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	// transformation
	/*glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(0.5f, 1.5f, 0.5f));
	shader.activate();
	shader.setMat4("transform", trans);*/

	mainJ.update();
	if (mainJ.isPresent()) {
		std::cout << mainJ.getName() << " is present." << std::endl;
	}

	x = 0.0f;
	y = 0.0f;
	z = 3.0f;
	while (!glfwWindowShouldClose(window)) {
		// process input
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// draw shapes
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		shader.activate();
		// set color
		//float timeValue = glfwGetTime();
		//float blueValue = (sin(timeValue) / 2.0f) + 0.5f;
		//shader.set4Float("ourColor", 0.0f, 0.0f, blueValue, 1.0f);
		//trans = glm::rotate(trans, glm::radians(timeValue / 100), glm::vec3(0.1f, 0.1f, 0.1f));
		//shader.setMat4("transform", trans);

		shader.setMat4("mouseTransform", mouseTransform);
		shader.setFloat("mixVal", mixVal);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
		//shader.setMat4("transform", trans);
		// draw second rectangle
		/*glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		shader.setMat4("transform", trans);*/

		// create transformation
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-55.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		view = glm::translate(view, glm::vec3(-x, -y, -z));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glBindVertexArray(0);

		// send new frame to window
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}

	// change mix value
	if (Keyboard::key(GLFW_KEY_UP)) {
		mixVal += .05f;
		if (mixVal > 1) {
			mixVal = 1.0f;
		}
	}
	if (Keyboard::key(GLFW_KEY_DOWN)) {
		mixVal -= .05f;
		if (mixVal < 0) {
			mixVal = 0.0f;
		}
	}

	// move camera
	float changeVal = 0.01f;
	if (Keyboard::key(GLFW_KEY_A)) {
		x -= changeVal;
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		x += changeVal;
	}

	if (Keyboard::key(GLFW_KEY_SPACE)) {
		y += changeVal;
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
		y -= changeVal;
	}

	if (Keyboard::key(GLFW_KEY_W)) {
		z -= changeVal;
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		z += changeVal;
	}

	// mouse
	/*if (Mouse::button(GLFW_MOUSE_BUTTON_LEFT)) {
		double x = Mouse::getMouseX();
		double y = Mouse::getMouseY();

		std::cout << x << ' ' << y << std::endl;

		mouseTransform = glm::mat4(1.0f);
		mouseTransform = glm::translate(mouseTransform, glm::vec3(x, y, 0.0f));
	}*/

	// joystick
	//mainJ.update();

	//float lx = mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_X);
	//float ly = -mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_Y);

	//// account for deadzone
	//if (std::abs(lx) > .05) {
	//	mouseTransform = glm::translate(mouseTransform, glm::vec3(lx / 10, 0.0f, 0.0f));
	//}

	//if (std::abs(ly) > .05) {
	//	mouseTransform = glm::translate(mouseTransform, glm::vec3(0.0f, ly / 10, 0.0f));
	//}
}