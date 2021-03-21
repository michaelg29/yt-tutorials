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

#include "../models/box.hpp"

#include "../../physics/collisionmodel.h"
#include "../../physics/rigidbody.h"

#include "../../algorithms/bounds.h"

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
    // pointer to the collision model
    CollisionModel* collision;
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

    // enable a collision model
    void enableCollisionModel();

    // add a mesh to the list
    void addMesh(Mesh* mesh);

    // render instance(s)
    virtual void render(Shader shader, float dt, Scene *scene, glm::mat4 model = glm::mat4(1.0f));

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

    // proces a custom mesh
    Mesh processMesh(BoundingRegion br,
        unsigned int noVertices, float* vertices,
        unsigned int noIndices, unsigned int* indices,
        bool calcTanVectors = true,
        unsigned int noCollisionPoints = 0, float* collisionPoints = NULL,
        unsigned int noCollisionFaces = 0, unsigned int* collisionIndices = NULL,
        bool pad = false);

    // load list of textures
    std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

    // VBOs for positions and sizes
    BufferObject posVBO;
    BufferObject sizeVBO;
};

#endif