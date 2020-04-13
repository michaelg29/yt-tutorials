#ifndef LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"
#include "../material.h"
#include "../shader.h"

#include <glm/glm.hpp>

class Lamp : public Cube {
public:
	glm::vec3 lightColor;

	// light strength values
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Lamp(glm::vec3 lightColor = glm::vec3(1.0f),
		glm::vec3 ambient = glm::vec3(1.0f),
		glm::vec3 diffuse = glm::vec3(1.0f),
		glm::vec3 specular = glm::vec3(1.0f),
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 size = glm::vec3(1.0f))
		: lightColor(lightColor),
		ambient(ambient),
		diffuse(diffuse),
		specular(specular),
		Cube(Material::white_plastic, pos, size) {}

	void render(Shader shader) {
		// set light color
		shader.set3Float("lightColor", lightColor);

		Cube::render(shader);
	}
};

#endif