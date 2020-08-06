#include "environment.h"

/*
    define constants in Environment class
*/

// up vector in world
glm::vec3 Environment::worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

// acceleration due to gravity
glm::vec3 Environment::gravitationalAcceleration = glm::vec3(0.0f, -9.81f, 0.0f);