/*
	CODE
	ENGINE (Scene)
	OPENGL
	GPU
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

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

#include "algorithms/states.hpp"

#include "scene.h"

Scene scene;

void processInput(double dt);

Camera cam;

//Joystick mainJ(0);

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

Sphere sphere(10);

int main() {
	std::cout << "Hello, OpenGL!" << std::endl;

	scene = Scene(3, 3, "OpenGL Tutorial", 800, 600);
	if (!scene.init()) {
		std::cout << "Could not open window" << std::endl;
		glfwTerminate();
		return -1;
	}

	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;

	// SHADERS===============================
	Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
	Shader shader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");

	// MODELS==============================
	Lamp lamp(4);
	scene.registerModel(&lamp);

	scene.registerModel(&sphere);

	Box box;
	box.init();

	// load all model data
	scene.loadModels();

	// LIGHTS
	DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) };
	scene.dirLight = &dirLight;

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

	PointLight pointLights[4];

	for (unsigned int i = 0; i < 4; i++) {
		pointLights[i] = {
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
		};
		scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}

	SpotLight spotLight = {
		cam.cameraPos, cam.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	};
	scene.spotLights.push_back(&spotLight);
	scene.activeSpotLights = 1;	// 0b00000001

	// instantiate instances
	scene.initInstances();

	// joystick recognition
	/*mainJ.update();
	if (mainJ.isPresent()) {
		std::cout << mainJ.getName() << " is present." << std::endl;
	}*/

	scene.prepare(box);

	while (!scene.shouldClose()) {
		// calculate dt
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// update screen values
		scene.update();

		// process input
		processInput(dt);

		// remove launch objects if too far
		for (int i = 0; i < sphere.currentNoInstances; i++) {
			if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) {
				scene.markForDeletion(sphere.instances[i]->instanceId);
			}
		}

		// render launch objects
		if (sphere.currentNoInstances > 0) {
			scene.renderShader(shader);
			scene.renderInstances(sphere.id, shader, dt);
		}

		// render lamps
		scene.renderShader(lampShader, false);
		scene.renderInstances(lamp.id, lampShader, dt);

		// render boxes
		scene.renderShader(boxShader, false);
		box.render(boxShader);

		// send new frame to window
		scene.newFrame(box);
		scene.clearDeadInstances();
	}

	// clean up objects
	scene.cleanup();
	return 0;
}

void launchItem(float dt) {
	RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, cam.cameraPos);
	if (rb) {
		// instance generated
		rb->transferEnergy(1000.0f, cam.cameraFront);
		rb->applyAcceleration(Environment::gravitationalAcceleration);
	}
}

void processInput(double dt) {
	scene.processInput(dt);

	// update flash light
	if (States::isIndexActive(&scene.activeSpotLights, 0)) {
		scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
		scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
	}

	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		scene.setShouldClose(true);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_L)) {
		States::toggleIndex(&scene.activeSpotLights, 0); // toggle spot light
	}

	if (Keyboard::keyWentDown(GLFW_KEY_F)) {
		launchItem(dt);
	}

	for (int i = 0; i < 4; i++) {
		if (Keyboard::keyWentDown(GLFW_KEY_1 + i)) {
			States::toggleIndex(&scene.activePointLights, i);
		}
	}
}