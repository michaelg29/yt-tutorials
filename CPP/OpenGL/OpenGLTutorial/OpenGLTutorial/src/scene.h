#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <jsoncpp/json.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/model.h"
#include "graphics/text.h"

#include "graphics/models/box.hpp"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"
#include "algorithms/trie.hpp"
#include "algorithms/octree.h"

// forward declarations
namespace Octree {
    class node;
}

class Model;

/*
    Scene class
    - ties together the many functions in the program (rendering, physics, collision, etc)
*/

class Scene {
public:
    // tries to store models/instances
    trie::Trie<Model*> models;
    trie::Trie<RigidBody*> instances;

    // list of instances that should be deleted
    std::vector<RigidBody*> instancesToDelete;

    // pointer to root node in octree
    Octree::node* octree;

    // map for logged variables
    jsoncpp::json variableLog;

    // freetype library
    FT_Library ft;
    trie::Trie<TextRenderer> fonts;

    /*
        callbacks
    */
    
    // window resize
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    /*
        constructors
    */
    
    // default
    Scene();

    // set with values
    Scene(int glfwVersionMajor, int glfwVersionMinor,
        const char* title, unsigned int scrWidth, unsigned int scrHeight);

    /*
        initialization
    */

    // to be called after constructor
    bool init();

    // to be called after instances have been generated/registered
    void prepare(Box &box);

    /*
        main loop methods
    */

    // process input
    void processInput(float dt);

    // update screen before each frame
    void update();

    // update screen after frame
    void newFrame(Box &box);

    // set uniform shader varaibles (lighting, etc)
    void renderShader(Shader shader, bool applyLighting = true);

    // render specified model's instances
    void renderInstances(std::string modelId, Shader shader, float dt);

    // render text
    void renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

    /*
        cleanup method
    */

    // called after main loop
    void cleanup();

    /*
        accessors
    */

    // determine if window should close
    bool shouldClose();

    // get current active camera in scene
    Camera* getActiveCamera();

    /*
        modifiers
    */

    // set if the window should close
    void setShouldClose(bool shouldClose);

    // set window background color
    void setWindowColor(float r, float g, float b, float a);

    /*
        Model/instance methods
    */

    // register model into model trie
    void registerModel(Model* model);

    // generate instance of specified model with physical parameters
    RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);

    // initialize model instances
    void initInstances();

    // load model data
    void loadModels();

    // delete instance
    void removeInstance(std::string instanceId);

    // mark instance for deletion
    void markForDeletion(std::string instanceId);

    // clear all instances marked for deletion
    void clearDeadInstances();

    // current instance id
    std::string currentId;

    // generate next instance id
    std::string generateId();

    /*
        lights
    */

    // list of point lights
    std::vector<PointLight*> pointLights;
    // acts as array of switches for point lights
    unsigned int activePointLights;

    // list of spot lights
    std::vector<SpotLight*> spotLights;
    // acts as array of switches for spot lights
    unsigned int activeSpotLights;

    // direction light
    DirLight* dirLight;
    // switch for directional light
    bool dirLightActive;

    /*
        camera
    */

    // list of cameras
    std::vector<Camera*> cameras;
    // index of active camera
    unsigned int activeCamera;

    // camera position/matrices
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 textProjection;
    glm::vec3 cameraPos;

protected:
    // window object
    GLFWwindow* window;

    // window vals
    const char* title;
    static unsigned int scrWidth;
    static unsigned int scrHeight;

    float bg[4]; // background color

    // GLFW info
    int glfwVersionMajor;
    int glfwVersionMinor;
};

#endif