#include "../model.h"
#include "../../io/camera.h"
#include "../../io/keyboard.h"

#include "../../scene.h"

#include "../../physics/environment.h"

class Gun : public Model {
public:
    Gun(unsigned int maxNoInstances)
        : Model("m4a1", BoundTypes::AABB, maxNoInstances, CONST_INSTANCES | NO_TEX) {}

    void init() {
        loadModel("assets/models/m4a1/scene.gltf");
    }

    void render(Shader shader, float dt, Scene *scene, bool setModel = false) {
        glm::mat4 model = glm::mat4(1.0f);

        // set position
        // multiply offset by unit vector in 2 directions
        instances[0]->pos = scene->getActiveCamera()->cameraPos + glm::vec3(scene->getActiveCamera()->cameraFront * 0.5f) - glm::vec3(scene->getActiveCamera()->cameraUp * 0.205f);
        model = glm::translate(model, instances[0]->pos);

        float theta;

        // rotate around camera right using dot product
        theta = acos(glm::dot(Environment::worldUp, scene->getActiveCamera()->cameraFront) /
            glm::length(scene->getActiveCamera()->cameraUp) / glm::length(scene->getActiveCamera()->cameraFront));
        model = glm::rotate(model, atanf(1) * 2 - theta, scene->getActiveCamera()->cameraRight); // offset by pi/2 radians bc angle btwn camFront and gunFront

        // rotate around cameraUp using dot product
        glm::vec2 front2d = glm::vec2(scene->getActiveCamera()->cameraFront.x, scene->getActiveCamera()->cameraFront.z);
        theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2d) / glm::length(front2d));
        model = glm::rotate(model, scene->getActiveCamera()->cameraFront.z < 0 ? theta : -theta, Environment::worldUp);

        // scale
        model = glm::scale(model, instances[0]->size);

        shader.setMat4("model", model);

        Model::render(shader, dt, scene, false);
    }
};