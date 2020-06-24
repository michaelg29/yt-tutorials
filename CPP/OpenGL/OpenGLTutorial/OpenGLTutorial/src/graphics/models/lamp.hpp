#ifndef LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"
#include "../material.h"
#include "../shader.h"
#include "../light.h"

#include <glm/glm.hpp>

class Lamp : public Cube {
public:
	glm::vec3 lightColor;

	Lamp(unsigned int maxNoInstances, glm::vec3 lightColor = glm::vec3(1.0f))
		: Cube(maxNoInstances) {
		id = "lamp";
		this->lightColor = lightColor;
	}

	void render(Shader shader, float dt, Scene *scene, bool setModel = true) {
		// set light color
		shader.set3Float("lightColor", lightColor);

		Cube::render(shader, dt, scene, setModel);
	}
};

#endif