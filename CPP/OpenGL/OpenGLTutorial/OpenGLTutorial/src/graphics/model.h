#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "mesh.h"

class Model {
public:
	Mesh mesh;

	Model();

	void init();

	void render(Shader shader);

	void cleanup();
};

#endif