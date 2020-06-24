#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "glmemory.hpp"

#include "models/box.hpp"

#include "../algorithms/bounds.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	/*
	v1.vec3.x|v1.vec3.y|v1.vec3.z|v1.vec2.x|v1.vec2.y|
	v2.vec3.x|v2.vec3.y|v2.vec3.z|v2.vec2.x|v2.vec2.y
	*/

	static std::vector<Vertex> genList(float* vertices, int noVertices);
};

class Mesh {
public:
	BoundingRegion br;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ArrayObject VAO;

	std::vector<Texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	// default constructor
	Mesh();

	// initialize as textured object
	Mesh(BoundingRegion br, std::vector<Texture> textures = {});

	// initialize as material object
	Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);

	// load vertex and index data
	void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void render(Shader shader, unsigned int noInstances);

	void cleanup();

private:
	unsigned int VBO, EBO;

	bool noTex;

	void setup();
};

#endif