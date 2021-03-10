#ifndef BRICKWALL_HPP
#define BRICKWALL_HPP

#include "plane.hpp"

class Brickwall : public Plane {
public:
    void init() {
        std::vector<Texture> textures = {
            Texture("assets/textures", "brickwall_diffuse.jpg", aiTextureType_DIFFUSE),
            Texture("assets/textures", "brickwall_normal.jpg", aiTextureType_NORMALS),
            Texture("assets/textures", "brickwall_specular.jpg", aiTextureType_SPECULAR)
        };

        for (Texture t : textures) {
            t.load();
        }

        Plane::init(textures);
    }
    /*
    void render(Shader shader, float dt, Scene* scene, glm::mat4 model = glm::mat4(1.0f)) {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), { 1.0f, 0.0f, 0.0f });

        Model::render(shader, dt, scene, model);
    }
    */
};

#endif