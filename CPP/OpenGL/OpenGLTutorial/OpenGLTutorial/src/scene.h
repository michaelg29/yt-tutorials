#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/model.h"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"

class Model;

class Scene {
public:
	std::map<std::string, Model*> models;
	std::map<std::string, std::pair<std::string, unsigned int>> instances;

	/*
		callbacks
	*/
	// window resize
	static void framebufferSizeCallback(GLFWwindow* widnow, int width, int height);

	/*
		constructor
	*/
	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);

	/*
		initialization
	*/
	bool init();

	/*
		main loop methods
	*/
	// process input
	void processInput(float dt);

	// update screen before each frame
	void update();

	// update screen after frame
	void newFrame();

	// set uniform shader varaibles (lighting, etc)
	void renderShader(Shader shader, bool applyLighting = true);

	void renderInstances(std::string modelId, Shader shader, float dt);

	/*
		cleanup method
	*/
	void cleanup();

	/*
		accessors
	*/
	bool shouldClose();

	Camera* getActiveCamera();

	/*
		modifiers
	*/
	void setShouldClose(bool shouldClose);

	void setWindowColor(float r, float g, float b, float a);

	/*
		Model/instance methods
	*/
	void registerModel(Model* model);

	std::string generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);

	void initInstances();

	void loadModels();

	void removeInstance(std::string instanceId);

	std::string currentId;
	std::string generateId();

	/*
		lights
	*/
	// list of point lights
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;
	// list of spot lights
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;
	// direction light
	DirLight* dirLight;
	bool dirLightActive;

	/*
		camera
	*/
	std::vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	// window object
	GLFWwindow* window;

	// window vals
	const char* title;
	static unsigned int scrWidth;
	static unsigned int scrHeight;

	float bg[4]; // background color

	// GLFW info
	int glfwVersionMajor;
	int glfwVersionMinor;
};

#endif