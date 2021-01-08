#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"
#include "../algorithms/bounds.h"
#include "framememory.hpp"

/*
    directional light (eg sun)
*/

struct DirLight {
    // direction (constant for all parallel rays)
    glm::vec3 direction;

    // light values
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    // bounding region for shadow
    BoundingRegion br;

    // transformation to light space (projection * view)
    glm::mat4 lightSpaceMatrix;

    // FBO for shadows
    FramebufferObject shadowFBO;

    // constructor
    DirLight(glm::vec3 direction,
        glm::vec4 ambient,
        glm::vec4 diffuse,
        glm::vec4 specular,
        BoundingRegion br);
    
    // render light into shader
    void render(Shader shader, unsigned int textureIdx);

    // update light space matrix
    void updateMatrices();
};

/*
    point light (eg light bulb)
*/

struct PointLight {
    // position
    glm::vec3 position;

    // attenuation constants
    float k0;
    float k1;
    float k2;

    // light values
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    // render light into shader
    void render(Shader shader, int idx);
};

/*
    spot light (flashlight)
*/

struct SpotLight {
    // position
    glm::vec3 position;
    // direction
    glm::vec3 direction;
    // up vector
    glm::vec3 up;

    // first level cut off
    float cutOff;
    // second level cut off
    float outerCutOff;

    // attenuation constants
    float k0;
    float k1;
    float k2;

    // light values
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    // bounds for the shadow
    float nearPlane;
    float farPlane;

    // light space transformation
    glm::mat4 lightSpaceMatrix;

    // FBO for shadows
    FramebufferObject shadowFBO;

    // constructor
    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
        float cutOff, float outerCutOff,
        float k0, float k1, float k2,
        glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
        float nearPlane, float farPlane);

    // render light into shader
    void render(Shader shader, int idx, unsigned int textureIdx);

    // update light space matrix
    void updateMatrices();
};

#endif