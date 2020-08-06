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

#include "models/box.hpp"

#include "../physics/rigidbody.h"
#include "../algorithms/bounds.h"

#include "../scene.h"

// model switches
#define DYNAMIC				(unsigned int)1 // 0b00000001
#define CONST_INSTANCES		(unsigned int)2 // 0b00000010
#define NO_TEX				(unsigned int)4	// 0b00000100

// forward declaration
class Scene;

/*
    class to represent model
*/

class Model {
public:
    // id of model in scene
    std::string id;

    // type of bounding region for all meshes
    BoundTypes boundType;

    // list of meshes
    std::vector<Mesh> meshes;
    // list of bounding regions (1 for each mesh)
    std::vector<BoundingRegion> boundingRegions;

    // list of instances
    std::vector<RigidBody*> instances;

    // maximum number of instances
    unsigned int maxNoInstances;
    // current number of instances
    unsigned int currentNoInstances;

    // combination of switches above
    unsigned int switches;

    /*
        constructor
    */

    // initialize with parameters
    Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags = 0);

    /*
        process functions
    */

    // initialize method (to be overriden)
    virtual void init();

    // load model from path
    void loadModel(std::string path);

    // render instance(s)
    virtual void render(Shader shader, float dt, Scene *scene, bool setModel = true);

    // free up memory
    void cleanup();

    /*
        instance methods
    */

    // generate instance with parameters
    RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos);

    // initialize memory for instances
    void initInstances();

    // remove instance at idx
    void removeInstance(unsigned int idx);

    // remove instance with id
    void removeInstance(std::string instanceId);

    // get index of instance with id
    unsigned int getIdx(std::string id);

protected:
    // true if doesn't have textures
    bool noTex;
    
    // directory containing object file
    std::string directory;

    // list of loaded textures
    std::vector<Texture> textures_loaded;

    /*
        model loading functions (ASSIMP)
    */

    // process node in object file
    void processNode(aiNode* node, const aiScene* scene);

    // process mesh in object file
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // load list of textures
    std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

    // VBOs for positions and sizes
    BufferObject posVBO;
    BufferObject sizeVBO;
};

#endif