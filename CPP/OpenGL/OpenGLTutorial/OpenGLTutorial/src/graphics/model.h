#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "mesh.h"

#include "../physics/rigidbody.h"

class Model {
public:
	RigidBody rb;
	glm::vec3 size;

	Model() {}
	Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), bool noTex = false);

	void loadModel(std::string path);

	void render(Shader shader, float dt, bool setModel = true);

	void cleanup();

protected:
	bool noTex;

	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};

#endif