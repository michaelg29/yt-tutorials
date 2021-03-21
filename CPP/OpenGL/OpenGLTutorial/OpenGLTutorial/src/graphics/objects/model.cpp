#include "model.h"

#include "../../physics/environment.h"

#include <iostream>

/*
    constructor
*/

// initialize with parameters
Model::Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags)
    : id(id), boundType(boundType), switches(flags), currentNoInstances(0), maxNoInstances(maxNoInstances) {}

/*
    process functions
*/

// initialize method (to be overriden)
void Model::init() {}

// load model from path
void Model::loadModel(std::string path) {
    // use ASSIMP to read file
    Assimp::Importer import;
    // triangulate = group indices in triangles, flip = flip textures on load
    const aiScene* scene = import.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // if no errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Could not load model at " << path << std::endl << import.GetErrorString() << std::endl;
        return;
    }

    // parse directory from path
    directory = path.substr(0, path.find_last_of("/"));

    // process root node
    processNode(scene->mRootNode, scene);
}

// render instance(s)
void Model::render(Shader shader, float dt, Scene* scene, glm::mat4 model) {
    // set model matrix
    shader.setMat4("model", model);
    shader.setMat3("normalModel", glm::transpose(glm::inverse(glm::mat3(model))));

    if (!States::isActive(&switches, CONST_INSTANCES)) {
        // dynamic instances - update VBO data

        // create list of each
        std::vector<glm::vec3> positions, sizes;

        // determine if instances are moving
        bool doUpdate = States::isActive(&switches, DYNAMIC);

        // iterate through each instance
        for (int i = 0; i < currentNoInstances; i++) {
            if (doUpdate) {
                // update Rigid Body
                instances[i]->update(dt);
                // activate moved switch
                States::activate(&instances[i]->state, INSTANCE_MOVED);
            }
            else {
                // deactivate moved switch
                States::deactivate(&instances[i]->state, INSTANCE_MOVED);
            }

            // add updates positions and sizes
            positions.push_back(instances[i]->pos);
            sizes.push_back(instances[i]->size);
        }

        // set position data
        posVBO.bind();
        posVBO.updateData<glm::vec3>(0, currentNoInstances, &positions[0]);

        // set size data
        sizeVBO.bind();
        sizeVBO.updateData<glm::vec3>(0, currentNoInstances, &sizes[0]);
    }

    // set shininess
    shader.setFloat("material.shininess", 0.5f);

    // render each mesh
    for (unsigned int i = 0, noMeshes = meshes.size(); i < noMeshes; i++) {
        meshes[i].render(shader, currentNoInstances);
    }
}

// free up memory
void Model::cleanup() {
    // free all instances
    for (unsigned int i = 0, len = instances.size(); i < len; i++) {
        if (instances[i]) {
            free(instances[i]);
        }
    }
    instances.clear();

    // cleanup each mesh
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].cleanup();
    }

    // free up memory for position and size VBOs
    posVBO.cleanup();
    sizeVBO.cleanup();
}

/*
    instance methods
*/

// generate instance with parameters
RigidBody* Model::generateInstance(glm::vec3 size, float mass, glm::vec3 pos) {
    if (currentNoInstances >= maxNoInstances) {
        // all slots filled
        return nullptr;
    }

    // instantiate new instance
    instances.push_back(new RigidBody(id, size, mass, pos));
    return instances[currentNoInstances++];
}

// initialize memory for instances
void Model::initInstances() {
    // default values
    glm::vec3* posData = nullptr;
    glm::vec3* sizeData = nullptr;
    GLenum usage = GL_DYNAMIC_DRAW;

    std::vector<glm::vec3> positions, sizes;

    if (States::isActive(&switches, CONST_INSTANCES)) {
        // instances won't change, set data pointers

        for (unsigned int i = 0; i < currentNoInstances; i++) {
            positions.push_back(instances[i]->pos);
            sizes.push_back(instances[i]->size);
        }

        if (positions.size() > 0) {
            posData = &positions[0];
            sizeData = &sizes[0];
        }

        usage = GL_STATIC_DRAW;
    }

    // generate positions VBO
    posVBO = BufferObject(GL_ARRAY_BUFFER);
    posVBO.generate();
    posVBO.bind();
    posVBO.setData<glm::vec3>(UPPER_BOUND, posData, usage);

    // generate size VBO
    sizeVBO = BufferObject(GL_ARRAY_BUFFER);
    sizeVBO.generate();
    sizeVBO.bind();
    sizeVBO.setData<glm::vec3>(UPPER_BOUND, sizeData, usage);

    // set attribute pointers for each mesh
    for (unsigned int i = 0, size = meshes.size(); i < size; i++) {
        meshes[i].VAO.bind();

        // set vertex attrib pointers
        // positions
        posVBO.bind();
        posVBO.setAttPointer<glm::vec3>(4, 3, GL_FLOAT, 1, 0, 1);
        // size
        sizeVBO.bind();
        sizeVBO.setAttPointer<glm::vec3>(5, 3, GL_FLOAT, 1, 0, 1);

        ArrayObject::clear();
    }
}

// remove instance at idx
void Model::removeInstance(unsigned int idx) {
    instances.erase(instances.begin() + idx);
    currentNoInstances--;
}

// remove instance with id
void Model::removeInstance(std::string instanceId) {
    int idx = getIdx(instanceId);
    if (idx != -1) {
        removeInstance(idx);
    }
}

// get index of instance with id
unsigned int Model::getIdx(std::string id) {
    // test each instance
    for (int i = 0; i < currentNoInstances; i++) {
        if (instances[i]->instanceId == id) {
            return i;
        }
    }
    return -1;
}

/*
    model loading functions (ASSIMP)
*/

// process node in object file
void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh newMesh = processMesh(mesh, scene);
        meshes.push_back(newMesh);
        boundingRegions.push_back(newMesh.br);
    }

    // process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

// process mesh in object file
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // setup bounding region
    BoundingRegion br(boundType);
    glm::vec3 min((float)(~0));		// min point = max float
    glm::vec3 max(-(float)(~0));	// max point = min float

    // vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // position
        vertex.pos = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // determine if outside of current min and max
        for (int j = 0; j < 3; j++) {
            // if smaller than min
            if (vertex.pos[j] < min[j]) min[j] = vertex.pos[j];
            // if larger than max
            if (vertex.pos[j] > max[j]) max[j] = vertex.pos[j];
        }

        // normal vectors
        vertex.normal = glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        );

        // textures
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else {
            vertex.texCoord = glm::vec2(0.0f);
        }

        // tangent vector
        vertex.tangent = {
            mesh->mTangents[i].x,
            mesh->mTangents[i].y,
            mesh->mTangents[i].z
        };

        vertices.push_back(vertex);
    }

    // process min/max for BR
    if (boundType == BoundTypes::AABB) {
        // assign max and min
        br.min = min;
        br.ogMin = min;
        br.max = max;
        br.ogMax = max;
    }
    else {
        // calculate max distance from the center
        br.center = BoundingRegion(min, max).calculateCenter();
        br.ogCenter = br.center;
        float maxRadiusSquared = 0.0f;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            float radiusSquared = 0.0f; // distance for this vertex
            for (int j = 0; j < 3; j++) {
                radiusSquared += (vertices[i].pos[j] - br.center[j]) * (vertices[i].pos[j] - br.center[j]);
            }
            if (radiusSquared > maxRadiusSquared) {
                // found new squared radius
                // a^2 > b^2 --> |a| > |b|
                maxRadiusSquared = radiusSquared;
            }
        }

        br.radius = sqrt(maxRadiusSquared);
        br.ogRadius = br.radius;
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // load data into mesh
    Mesh ret;

    // process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (States::isActive<unsigned int>(&switches, NO_TEX)) {
            // use materials

            // 1. diffuse colors
            aiColor4D diff(1.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
            // 2. specular colors
            aiColor4D spec(1.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

            ret = Mesh(br, diff, spec);
        }
        else {
            // use textures

            // 1. diffuse maps
            std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            // .obj, use aiTextureType_HEIGHT
            std::vector<Texture> normalMaps = loadTextures(material, aiTextureType_NORMALS);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            ret = Mesh(br, textures);
        }
    }

    // load vertex and index data
    ret.loadData(vertices, indices);
    return ret;
}

// load list of textures
std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // prevent duplicate loading
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            // not loaded yet
            Texture tex(directory, str.C_Str(), type);
            tex.load(false);
            textures.push_back(tex);
            textures_loaded.push_back(tex);
        }
    }

    return textures;
}