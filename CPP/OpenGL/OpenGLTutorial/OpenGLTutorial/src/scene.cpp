#include "scene.h"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;



/*
    callbacks
*/

// window resize
void Scene::framebufferSizeCallback(GLFWwindow* widnow, int width, int height) {
    glViewport(0, 0, width, height);
    // update variables
    Scene::scrWidth = width;
    Scene::scrHeight = height;
}

/*
    constructors
*/

// default
Scene::Scene() 
    : currentId("aaaaaaaa") {}

// set with values
Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
    const char* title, unsigned int scrWidth, unsigned int scrHeight)
    : glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), // GLFW version
    title(title), // window title
    // default indices/vals
    activeCamera(-1), 
    activePointLights(0), activeSpotLights(0),
    currentId("aaaaaaaa") {
    
    // window dimensions
    Scene::scrWidth = scrWidth;
    Scene::scrHeight = scrHeight;

    // window color
    setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
}

/*
    initialization
*/

// to be called after constructor
bool Scene::init() {
    glfwInit();

    // set version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // initialize window
    window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
    if (window == NULL) {
        // not created
        return false;
    }
    glfwMakeContextCurrent(window);

    // set GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    // setup screen
    glViewport(0, 0, scrWidth, scrHeight);

    /*
        callbacks
    */
    // frame size
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // key pressed
    glfwSetKeyCallback(window, Keyboard::keyCallback);
    // cursor moved
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    // mouse btn pressed
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    // mouse scroll
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

    /*
        set rendering parameters
    */

    // depth testing
    glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera (back of object)

    // blending for text textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // stencil testing
    glEnable(GL_STENCIL_TEST);
    // keep fragments if either stencil or depth fails, replace if both pass
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

    /*
        init octree
    */
    octree = new Octree::node(BoundingRegion(glm::vec3(-16.0f), glm::vec3(16.0f)));

    /*
        initialize freetype library
    */
    if (FT_Init_FreeType(&ft)) {
        std::cout << "Could not init FreeType library" << std::endl;
        return false;
    }

    // insert font
    fonts.insert("comic", TextRenderer(32));
    if (!fonts["comic"].loadFont(ft, "assets/fonts/comic.ttf")) {
        std::cout << "Could not load font" << std::endl;
        return false;
    }

    FT_Done_FreeType(ft);

    // setup lighting values
    variableLog["useBlinn"] = true;
    variableLog["useGamma"] = true;
    variableLog["dispOutline"] = false;

    return true;
}

// to be called after instances have been generated/registered
void Scene::prepare(Box &box) {
    // process current instances
    octree->update(box);
}

/*
    main loop methods
*/

// process input
void Scene::processInput(float dt) {
    if (activeCamera != -1 && activeCamera < cameras.size()) {
        // active camera exists

        // set camera direction
        double dx = Mouse::getDX(), dy = Mouse::getDY();
        if (dx != 0 || dy != 0) {
            cameras[activeCamera]->updateCameraDirection(dx, dy);
        }

        // set camera zoom
        double scrollDy = Mouse::getScrollDY();
        if (scrollDy != 0) {
            cameras[activeCamera]->updateCameraZoom(scrollDy);
        }

        // set camera pos
        if (Keyboard::key(GLFW_KEY_W)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
        }
        if (Keyboard::key(GLFW_KEY_S)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
        }
        if (Keyboard::key(GLFW_KEY_D)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
        }
        if (Keyboard::key(GLFW_KEY_A)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
        }
        if (Keyboard::key(GLFW_KEY_SPACE)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
        }
        if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
        }

        // set matrices
        view = cameras[activeCamera]->getViewMatrix();
        projection = glm::perspective(
            glm::radians(cameras[activeCamera]->getZoom()),	    // FOV
            (float)scrWidth / (float)scrHeight,					// aspect ratio
            0.1f, 100.0f										// near and far bounds
        );
        textProjection = glm::ortho(0.0f, (float)scrWidth, 0.0f, (float)scrHeight);

        // set pos
        cameraPos = cameras[activeCamera]->cameraPos;

        // update blinn parameter if necessary
        if (Keyboard::keyWentDown(GLFW_KEY_B)) {
            variableLog["useBlinn"] = !variableLog["useBlinn"].val<bool>();
        }

        // update gamma parameter if necessary
        if (Keyboard::keyWentDown(GLFW_KEY_G)) {
            variableLog["useGamma"] = !variableLog["useGamma"].val<bool>();
        }

        // update outline parameter if necessary
        if (Keyboard::keyWentDown(GLFW_KEY_O)) {
            variableLog["dispOutline"] = !variableLog["dispOutline"].val<bool>();
        }
    }
}

// update screen before each frame
void Scene::update() {
    // set background color
    glClearColor(bg[0], bg[1], bg[2], bg[4]);
    // clear occupied bits
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// update screen after frame
void Scene::newFrame(Box &box) {
    box.positions.clear();
    box.sizes.clear();

    // process pending objects
    octree->processPending();
    octree->update(box);

    // send new frame to window
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// set uniform shader varaibles (lighting, etc)
void Scene::renderShader(Shader shader, bool applyLighting) {
    // activate shader
    shader.activate();

    // set camera values
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.set3Float("viewPos", cameraPos);

    // lighting
    if (applyLighting) {
        // point lights
        unsigned int noLights = pointLights.size();
        unsigned int noActiveLights = 0;
        for (unsigned int i = 0; i < noLights; i++) {
            if (States::isIndexActive(&activePointLights, i)) {
                // i'th light is active
                pointLights[i]->render(shader, noActiveLights);
                noActiveLights++;
            }
        }
        shader.setInt("noPointLights", noActiveLights);

        // spot lights
        noLights = spotLights.size();
        noActiveLights = 0;
        for (unsigned int i = 0; i < noLights; i++) {
            if (States::isIndexActive(&activeSpotLights, i)) {
                // i'th spot light active
                spotLights[i]->render(shader, noActiveLights);
                noActiveLights++;
            }
        }
        shader.setInt("noSpotLights", noActiveLights);

        // directional light
        dirLight->render(shader);

        shader.setBool("useBlinn", variableLog["useBlinn"].val<bool>());
        shader.setBool("useGamma", variableLog["useGamma"].val<bool>());
    }
}

// render specified model's instances
void Scene::renderInstances(std::string modelId, Shader shader, float dt) {
    // render each mesh in specified model
    shader.activate();
    models[modelId]->render(shader, dt, this);
}

// render text
void Scene::renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color) {
    shader.activate();
    shader.setMat4("projection", textProjection);

    fonts[font].render(shader, text, x, y, scale, color);
}

/*
    cleanup method
*/

// called after main loop
void Scene::cleanup() {
    // clean all models
    models.traverse([](Model* model) -> void {
        model->cleanup();
    });

    // clean up model and instances tries
    models.cleanup();
    instances.cleanup();

    // cleanup fonts
    fonts.traverse([](TextRenderer tr) -> void {
        tr.cleanup();
    });

    // clean up fonts trie
    fonts.cleanup();

    // destroy octree
    octree->destroy();

    // terminate glfw
    glfwTerminate();
}

/*
    accessors
*/

// determine if window should close
bool Scene::shouldClose() {
    return glfwWindowShouldClose(window);
}

// get current active camera in scene
Camera* Scene::getActiveCamera() {
    // if activate camera exists, return the one at that index
    return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

/*
    modifiers
*/

// set if the window should close
void Scene::setShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}

// set window background color
void Scene::setWindowColor(float r, float g, float b, float a) {
    // set rgba vals of the background color
    bg[0] = r;
    bg[1] = g;
    bg[2] = b;
    bg[3] = a;
}

/*
    Model/instance methods
*/

// register model into model trie
void Scene::registerModel(Model* model) {
    models.insert(model->id, model);
}

// generate instance of specified model with physical parameters
RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos) {
    // generate new rigid body
    RigidBody* rb = models[modelId]->generateInstance(size, mass, pos);
    if (rb) {
        // successfully generated, set new and unique id for instance
        std::string id = generateId();
        rb->instanceId = id;
        // insert into trie
        instances.insert(id, rb);
        // insert into pending queue
        octree->addToPending(rb, models);
        return rb;
    }
    return nullptr;
}

// initialize model instances
void Scene::initInstances() {
    // initialize all instances for each model
    models.traverse([](Model* model) -> void {
        model->initInstances();
    });
}

// load model data
void Scene::loadModels() {
    // initialize each model
    models.traverse([](Model* model) -> void {
        model->init();
    });
}

// delete instance
void Scene::removeInstance(std::string instanceId) {
    // get instance's model
    std::string targetModel = instances[instanceId]->modelId;

    // delete instance from model
    models[targetModel]->removeInstance(instanceId);

    // remove from trie
    instances[instanceId] = nullptr;
    instances.erase(instanceId);
}

// mark instance for deletion
void Scene::markForDeletion(std::string instanceId) {
    // activate kill switch
    States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
    // push to list
    instancesToDelete.push_back(instances[instanceId]);
}

// clear all instances marked for deletion
void Scene::clearDeadInstances() {
    for (RigidBody* rb : instancesToDelete) {
        removeInstance(rb->instanceId);
    }
    instancesToDelete.clear();
}

// generate next instance id
std::string Scene::generateId() {
    for (int i = currentId.length() - 1; i >= 0; i--) {
        if ((int)currentId[i] != (int)'z') {
            // increment then break
            currentId[i] = (char)(((int)currentId[i]) + 1);
            break;
        }
        else {
            currentId[i] = 'a';
        }
    }
    return currentId;
}