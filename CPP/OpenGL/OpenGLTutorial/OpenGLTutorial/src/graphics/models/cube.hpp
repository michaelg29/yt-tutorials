#ifndef CUBE_H
#define CUBE_H

#include "../model.h"

class Cube : public Model {
public:
	glm::vec3 pos;
	glm::vec3 size;

	Cube(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f))
		: pos(pos), size(size) {}

	void init() {
		int noVertices = 36;

		float vertices[] = {
		//	position				texture coordinates
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f
		};

		std::vector<unsigned int> indices(noVertices);
		for (unsigned int i = 0; i < noVertices; i++) {
			indices[i] = i;
		}

		Texture tex0("assets/image1.jpg", "texture0");
		tex0.load();
		Texture tex1("assets/image2.png", "texture1");
		tex1.load();

		mesh = Mesh(Vertex::genList(vertices, noVertices), indices, { tex0, tex1 });
	}

	void render(Shader shader) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, size);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-55.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setMat4("model", model);

		Model::render(shader);
	}
};

#endif