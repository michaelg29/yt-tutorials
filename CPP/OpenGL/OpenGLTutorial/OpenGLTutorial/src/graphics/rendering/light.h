#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"
#include "../../algorithms/bounds.h"
#include "../memory/framememory.hpp"

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

    // default constructor
    DirLight();

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

    // bounds
    float nearPlane;
    float farPlane;

    // list of view matrices
    glm::mat4 lightSpaceMatrices[6];

    // FBO for shadows
    FramebufferObject shadowFBO;

    // default constructor
    PointLight();

    // constructor
    PointLight(glm::vec3 position,
        float k0, float k1, float k2,
        glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
        float nearPlane, float farPlane);

    // render light into shader
    void render(Shader shader, int idx, unsigned int textureIdx);

    // update light space matrices
    void updateMatrices();

    // list of directions
    static glm::vec3 directions[6];

    // list of up vectors
    static glm::vec3 ups[6];
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

    // default constructor
    SpotLight();

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