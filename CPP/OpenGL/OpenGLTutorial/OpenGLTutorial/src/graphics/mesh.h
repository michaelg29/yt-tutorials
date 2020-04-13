#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;

	std::vector<Texture> textures;

	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});

	void render(Shader shader);

	void cleanup();

private:
	unsigned int VBO, EBO;

	void setup();
};

#endif