/*
    CODE
    ENGINE (Scene)
    OPENGL
    GPU
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/light.h"
#include "graphics/cubemap.h"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "algorithms/states.hpp"

#include "scene.h"

Scene scene;

void processInput(double dt);

Camera cam;

//Joystick mainJ(0);

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

Sphere sphere(10);

int main() {
    std::cout << "Hello, OpenGL!" << std::endl;

    // construct scene
    scene = Scene(3, 3, "OpenGL Tutorial", 800, 600);
    // test if GLFW successfully started and created window
    if (!scene.init()) {
        std::cout << "Could not open window" << std::endl;
        scene.cleanup();
        return -1;
    }

    // set camera
    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;

    // SHADERS===============================
    Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
    Shader shader("assets/instanced/instanced.vs", "assets/object.fs");
    Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");
    Shader textShader("assets/text.vs", "assets/text.fs");
    Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/sky.fs");
    skyboxShader.activate();
    skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
    skyboxShader.set3Float("max", 0.945f, 1.000f, 0.682f);

    // SKYBOX=============================
    Cubemap skybox;
    skybox.init();
    //skybox.loadTextures("assets/skybox");

    // MODELS==============================
    Lamp lamp(4);
    scene.registerModel(&lamp);

    scene.registerModel(&sphere);

    Box box;
    box.init();

    // load all model data
    scene.loadModels();

    // LIGHTS==============================

    // directional light
    DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) };
    scene.dirLight = &dirLight;

    // point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.09f;
    float k2 = 0.032f;

    PointLight pointLights[4];

    for (unsigned int i = 0; i < 4; i++) {
        pointLights[i] = {
            pointLightPositions[i],
            k0, k1, k2,
            ambient, diffuse, specular
        };
        // create physical model for each lamp
        scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);
        // add lamp to scene's light source
        scene.pointLights.push_back(&pointLights[i]);
        // activate lamp in scene
        States::activate(&scene.activePointLights, i);
    }

    // spot light
    SpotLight spotLight = {
        cam.cameraPos, cam.cameraFront,
        glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
        1.0f, 0.07f, 0.032f,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
    };
    scene.spotLights.push_back(&spotLight);
    scene.activeSpotLights = 1;	// 0b00000001

    // instantiate instances
    scene.initInstances();

    // finish preparations (octree, etc)
    scene.prepare(box);

    // joystick recognition
    /*mainJ.update();
    if (mainJ.isPresent()) {
        std::cout << mainJ.getName() << " is present." << std::endl;
    }*/

    scene.variableLog["time"] = (double)0.0;

    while (!scene.shouldClose()) {
        // calculate dt
        double currentTime = glfwGetTime();
        dt = currentTime - lastFrame;
        lastFrame = currentTime;

        scene.variableLog["time"] += dt;
        scene.variableLog["fps"] = 1 / dt;

        // update screen values
        scene.update();

        // process input
        processInput(dt);

        // render skybox
        skyboxShader.activate();
        skyboxShader.setFloat("time", scene.variableLog["time"].val<float>());
        skybox.render(skyboxShader, &scene);

        scene.renderText("comic", textShader, "Hello, OpenGL!", 50.0f, 50.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        scene.renderText("comic", textShader, "Time: " + scene.variableLog["time"].dump(), 50.0f, 550.0f, glm::vec2(1.0f), glm::vec3(0.0f));
        scene.renderText("comic", textShader, "FPS: " + scene.variableLog["fps"].dump(), 50.0f, 550.0f - 40.0f, glm::vec2(1.0f), glm::vec3(0.0f));

        // remove launch objects if too far
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);
            }
        }

        // render launch objects
        if (sphere.currentNoInstances > 0) {
            scene.renderShader(shader);
            scene.renderInstances(sphere.id, shader, dt);
        }

        // render lamps
        scene.renderShader(lampShader, false);
        scene.renderInstances(lamp.id, lampShader, dt);

        // render boxes
        scene.renderShader(boxShader, false);
        box.render(boxShader);

        // send new frame to window
        scene.newFrame(box);

        // clear instances that have been marked for deletion
        scene.clearDeadInstances();
    }

    // clean up objects
    skybox.cleanup();
    scene.cleanup();
    return 0;
}

void launchItem(float dt) {
    RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, cam.cameraPos);
    if (rb) {
        // instance generated successfully
        rb->transferEnergy(1000.0f, cam.cameraFront);
        rb->applyAcceleration(Environment::gravitationalAcceleration);
    }
}

void processInput(double dt) {
    // process input with cameras
    scene.processInput(dt);

    // close window
    if (Keyboard::key(GLFW_KEY_ESCAPE)) {
        scene.setShouldClose(true);
    }

    // update flash light
    if (States::isIndexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
    }

    if (Keyboard::keyWentDown(GLFW_KEY_L)) {
        States::toggleIndex(&scene.activeSpotLights, 0); // toggle spot light
    }

    // launch sphere
    if (Keyboard::keyWentDown(GLFW_KEY_F)) {
        launchItem(dt);
    }

    // determine if each lamp should be toggled
    for (int i = 0; i < 4; i++) {
        if (Keyboard::keyWentDown(GLFW_KEY_1 + i)) {
            States::toggleIndex(&scene.activePointLights, i);
        }
    }
}