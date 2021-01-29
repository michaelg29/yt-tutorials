#ifndef PLANE_HPP
#define PLANE_HPP

#include "../model.h"

class Plane : public Model {
public:
	Plane()
		: Model("plane", BoundTypes::AABB, 1, CONST_INSTANCES) {}

	void init(std::vector<Texture> textures) {
		int noVertices = 4;

		float quadVertices[] = {
			// position			normal				texcoord
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
			-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
		};

		std::vector<unsigned int> indices = {
			0, 1, 3,
			1, 2, 3
		};

		BoundingRegion br(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f));

		Mesh ret(br, textures);
		ret.loadData(Vertex::genList(quadVertices, noVertices), indices, true);

		meshes.push_back(ret);
		boundingRegions.push_back(br);
	}
};

#endif