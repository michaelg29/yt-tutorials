#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/light.h"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "scene.h"

Scene scene;

void processInput(double dt);

Joystick mainJ(0);
Camera Camera::defaultCamera(glm::vec3(0.0f, 0.0f, 0.0f));

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

bool flashlightOn = false;

SphereArray launchObjects;

int main() {
	std::cout << "Hello, OpenGL!" << std::endl;

	scene = Scene(3, 3, "OpenGL Tutorial", 800, 600);
	if (!scene.init()) {
		std::cout << "Could not open window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// SHADERS===============================
	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
	Shader launchShader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");

	// MODELS==============================
	launchObjects.init();

	Box box;
	box.init();

	Model m(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(0.05f));
	m.loadModel("assets/models/lotr_troll/scene.gltf");

	// LIGHTS
	DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) };

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.09f;
	float k2 = 0.032f;

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++) {
		lamps.lightInstances.push_back({
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
			});
	}

	SpotLight s = {
		Camera::defaultCamera.cameraPos, Camera::defaultCamera.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	};

	// joystick recognition
	/*mainJ.update();
	if (mainJ.isPresent()) {
		std::cout << mainJ.getName() << " is present." << std::endl;
	}*/

	while (!screen.shouldClose()) {
		box.positions.clear();
		box.sizes.clear();

		// calculate dt
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// process input
		processInput(dt);

		// update screen values
		screen.update();

		// shader variables
		shader.activate();
		launchShader.activate();

		// camera view position
		shader.activate();
		shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		launchShader.activate();
		launchShader.set3Float("viewPos", Camera::defaultCamera.cameraPos);

		// render lights in shader
		shader.activate();
		dirLight.render(shader);
		launchShader.activate();
		dirLight.render(launchShader);

		for (unsigned int i = 0; i < 4; i++) {
			shader.activate();
			lamps.lightInstances[i].render(shader, i);
			launchShader.activate();
			lamps.lightInstances[i].render(launchShader, i);
		}
		shader.activate();
		shader.setInt("noPointLights", 4);
		launchShader.activate();
		launchShader.setInt("noPointLights", 4);

		if (flashlightOn) {
			s.position = Camera::defaultCamera.cameraPos;
			s.direction = Camera::defaultCamera.cameraFront;
			shader.activate();
			s.render(shader, 0);
			shader.setInt("noSpotLights", 1);
			launchShader.activate();
			s.render(launchShader, 0);
			launchShader.setInt("noSpotLights", 1);
		}
		else {
			shader.activate();
			shader.setInt("noSpotLights", 0);
			launchShader.activate();
			launchShader.setInt("noSpotLights", 0);
		}

		// create transformation
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(
			glm::radians(Camera::defaultCamera.zoom), 
			(float)Screen::SCR_WIDTH / (float)Screen::SCR_HEIGHT, 0.1f, 100.0f);

		shader.activate();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		m.render(shader, dt, &box);

		// launch objects
		std::stack<int> removeObjects;
		for (int i = 0; i < launchObjects.instances.size(); i++) {
			if (glm::length(Camera::defaultCamera.cameraPos - launchObjects.instances[i].pos) > 50.0f) {
				removeObjects.push(i);
				continue;
			}
		}
		for (int i = 0; i < removeObjects.size(); i++) {
			launchObjects.instances.erase(launchObjects.instances.begin() + removeObjects.top());
			removeObjects.pop();
		}

		if (launchObjects.instances.size() > 0) {
			launchShader.activate();
			launchShader.setMat4("view", view);
			launchShader.setMat4("projection", projection);
			launchObjects.render(launchShader, dt, &box);
		}

		// lamps
		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lamps.render(lampShader, dt, &box);

		// render boxes
		if (box.positions.size() > 0) {
			// instances exist
			boxShader.activate();
			boxShader.setMat4("view", view);
			boxShader.setMat4("projection", projection);
			box.render(boxShader);
		}

		// send new frame to window
		screen.newFrame();
		glfwPollEvents();
	}

	// clean up objects
	lamps.cleanup();
	box.cleanup();
	launchObjects.cleanup();
	m.cleanup();

	glfwTerminate();
	return 0;
}

void launchItem(float dt) {
	RigidBody rb(1.0f, Camera::defaultCamera.cameraPos);
	rb.transferEnergy(100.0f, Camera::defaultCamera.cameraFront);
	rb.applyAcceleration(Environment::gravitationalAcceleration);
	launchObjects.instances.push_back(rb);
}

void processInput(double dt) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		screen.setShouldClose(true);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_L)) {
		flashlightOn = !flashlightOn;
	}

	// move camera
	if (Keyboard::key(GLFW_KEY_W)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHT, dt);
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::LEFT, dt);
	}
	if (Keyboard::key(GLFW_KEY_SPACE)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::UP, dt);
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
		Camera::defaultCamera.updateCameraPos(CameraDirection::DOWN, dt);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_F)) {
		launchItem(dt);
	}
}