#ifndef LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"
#include "../rendering/material.h"

#include <glm/glm.hpp>

class Lamp : public Cube {
public:
    glm::vec3 lightColor;

    Lamp(unsigned int maxNoInstances, glm::vec3 lightColor = glm::vec3(1.0f))
        : Cube(maxNoInstances, Material::white_rubber) {
        id = "lamp";
        this->lightColor = lightColor;
    }
};

#endif