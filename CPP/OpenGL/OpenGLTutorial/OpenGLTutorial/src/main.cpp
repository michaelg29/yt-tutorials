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
#include "graphics/models/plane.hpp"

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
    
    Shader outlineShader("assets/outline.vs", "assets/outline.fs");
    Shader bufferShader("assets/buffer.vs", "assets/buffer.fs");
    
    //Shader textShader("assets/text.vs", "assets/text.fs");
    //Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/sky.fs");
    //skyboxShader.activate();
    //skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
    //skyboxShader.set3Float("max", 0.945f, 1.000f, 0.682f);

    // SKYBOX=============================
    //Cubemap skybox;
    //skybox.init();
    //skybox.loadTextures("assets/skybox");

    // MODELS==============================
    Lamp lamp(4);
    scene.registerModel(&lamp);

    scene.registerModel(&sphere);

    Cube cube(10);
    scene.registerModel(&cube);

    Box box;
    box.init();

    // FBO
    const GLuint BUFFER_WIDTH = 800, BUFFER_HEIGHT = 600;
    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    // initialize texture
    Texture bufferTex("bufferTex");

    // setup texture values
    bufferTex.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, BUFFER_WIDTH, BUFFER_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // attach texture to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferTex.id, 0);

    // renderbuffer to store color buffer unformatted
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // allocate memory for rbo
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, BUFFER_WIDTH, BUFFER_HEIGHT);
    // attach renderbuffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR with framebuffer" << std::endl;
    }

    glBindBuffer(GL_FRAMEBUFFER, 0); // rebind default framebuffer

    // setup plane to display texture
    Plane map;
    map.init(bufferTex);
    scene.registerModel(&map);

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
        States::activateIndex(&scene.activePointLights, i);
    }

    // spot light
    SpotLight spotLight = {
        cam.cameraPos, cam.cameraFront,
        glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
        1.0f, 0.0014f, 0.000007f,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
    };
    scene.spotLights.push_back(&spotLight);
    scene.activeSpotLights = 1;	// 0b00000001

    scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));
    glm::vec3 cubePositions[] = {
        { 1.0f, 3.0f, -5.0f },
        { -7.25f, 2.1f, 1.5f },
        { -15.0f, 2.55f, 9.0f },
        { 4.0f, -3.5f, 5.0f },
        { 2.8f, 1.9f, -6.2f },
        { 3.5f, 6.3f, -1.0f },
        { -3.4f, 10.9f, -5.5f },
        { 10.0f, -2.0f, 13.2f },
        { 2.1f, 7.9f, -8.3f }
    };
    for (unsigned int i = 0; i < 9; i++) {
        scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
    }

    // instantiate texture quad
    scene.generateInstance(map.id, glm::vec3(2.0f, 2.0f, 0.0f), 0.0f, glm::vec3(0.0f));

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

        /*
            render scene to the custom framebuffer
        */
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // render skybox
        //skyboxShader.activate();
        //skyboxShader.setFloat("time", scene.variableLog["time"].val<float>());
        //skybox.render(skyboxShader, &scene);

        //scene.renderText("comic", textShader, "Hello, OpenGL!", 50.0f, 50.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        //scene.renderText("comic", textShader, "Time: " + scene.variableLog["time"].dump(), 50.0f, 550.0f, glm::vec2(1.0f), glm::vec3(0.0f));
        //scene.renderText("comic", textShader, "FPS: " + scene.variableLog["fps"].dump(), 50.0f, 550.0f - 40.0f, glm::vec2(1.0f), glm::vec3(0.0f));

        // render lamps
        scene.renderShader(lampShader, false);
        scene.renderInstances(lamp.id, lampShader, dt);

        // remove launch objects if too far
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);
            }
        }

        if (scene.variableLog["dispOutline"].val<bool>()) {
            glStencilMask(0x00); // disable writing to stencil buffer for spheres 
        }

        // render launch objects
        scene.renderShader(shader);
        if (sphere.currentNoInstances > 0) {
            scene.renderInstances(sphere.id, shader, dt);
        }
        
        if (scene.variableLog["dispOutline"].val<bool>()) {
            // always write to stencil buffer with cubes
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF); // always write to buffer
            scene.renderInstances(cube.id, shader, dt);

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // render fragments if different than what is stored
            glStencilMask(0x00); // disable writing
            glDisable(GL_DEPTH_TEST); // disable depth test so outlines are displayed behind

            // draw outlines of the cubes
            scene.renderShader(outlineShader, false);
            scene.renderInstances(cube.id, outlineShader, dt);

            // reset values
            glStencilFunc(GL_ALWAYS, 1, 0xFF); // every fragment written to stencil buffer
            glStencilMask(0xFF); // write always
            glEnable(GL_DEPTH_TEST); // re-enable depth testing
        }
        else {
            // render cubes normally
            scene.renderInstances(cube.id, shader, dt);
        }

        // render boxes
        //scene.renderShader(boxShader, false);
        //box.render(boxShader);

        /*
            render texture
        */

        // rebind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, 800, 600);

        // render quad
        scene.renderInstances(map.id, bufferShader, dt);

        // send new frame to window
        scene.newFrame(box);

        // clear instances that have been marked for deletion
        scene.clearDeadInstances();
    }

    // clean up objects
    //skybox.cleanup();
    scene.cleanup();
    return 0;
}

void launchItem(float dt) {
    RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, cam.cameraPos);
    if (rb) {
        // instance generated successfully
        rb->transferEnergy(50.0f, cam.cameraFront);
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