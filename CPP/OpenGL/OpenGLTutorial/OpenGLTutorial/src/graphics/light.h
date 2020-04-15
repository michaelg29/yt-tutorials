#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"

struct DirLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	void render(Shader shader);
};

struct PointLight {
	glm::vec3 position;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

#endif