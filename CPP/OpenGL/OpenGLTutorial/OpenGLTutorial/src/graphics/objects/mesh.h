#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "../rendering/shader.h"
#include "../rendering/texture.h"
#include "../rendering/material.h"

#include "../memory/vertexmemory.hpp"

#include "../models/box.hpp"

#include "../../algorithms/bounds.h"

#include "../../physics/collisionmesh.h"

/*
    structure storing values for each vertex
*/

struct Vertex {
    /*
        vertex values
    */
    // position
    glm::vec3 pos;
    // normal vector
    glm::vec3 normal;
    // texture coordinate
    glm::vec2 texCoord;
    // tangent vector
    glm::vec3 tangent;

    // generate list of vertices
    static std::vector<Vertex> genList(float* vertices, int noVertices);

    // calculate tangent vectors for each face
    static void calcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices);
};

/*
    class representing Mesh
*/

class Mesh {
public:
    // Bounding region for mesh
    BoundingRegion br;
    // pointer to the attached collision mesh
    CollisionMesh* collision;

    // list of vertices
    std::vector<Vertex> vertices;
    // list of indices
    std::vector<unsigned int> indices;
    // vertex array object pointing to all data for the mesh
    ArrayObject VAO;

    // texture list
    std::vector<Texture> textures;
    // material diffuse value
    aiColor4D diffuse;
    // material specular value
    aiColor4D specular;

    /*
        constructors
    */

    // default
    Mesh();

    // intialize with a bounding region
    Mesh(BoundingRegion br);

    // initialize as textured object
    Mesh(BoundingRegion br, std::vector<Texture> textures);

    // initialize as material object
    Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);

    // initialize with a material
    Mesh(BoundingRegion br, Material m);

    // load vertex and index data
    void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool pad = false);

    // setup collision mesh
    void loadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);

    // setup textures
    void setupTextures(std::vector<Texture> textures);

    // setup material colors
    void setupColors(aiColor4D diff, aiColor4D spec);

    // set material structure
    void setupMaterial(Material mat);

    // render number of instances using shader
    void render(Shader shader, unsigned int noInstances);

    // free up memory
    void cleanup();

private:
    // true if has only materials
    bool noTex;

    // setup data with buffers
    void setup();
};

#endif