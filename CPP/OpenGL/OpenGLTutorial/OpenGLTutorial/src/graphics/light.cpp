#include "light.h"

// render directional light into shader
void DirLight::render(Shader shader) {
    // set direction
    shader.set3Float("dirLight.direction", direction);

    // set lighting values
    shader.set4Float("dirLight.ambient", ambient);
    shader.set4Float("dirLight.diffuse", diffuse);
    shader.set4Float("dirLight.specular", specular);
}

// render point light into shader
void PointLight::render(Shader shader, int idx) {
    // get name with index in array
    std::string name = "pointLights[" + std::to_string(idx) + "]";

    // set position
    shader.set3Float(name + ".position", position);

    // set attenuation constants
    shader.setFloat(name + ".k0", k0);
    shader.setFloat(name + ".k1", k1);
    shader.setFloat(name + ".k2", k2);

    // set lighting values
    shader.set4Float(name + ".ambient", ambient);
    shader.set4Float(name + ".diffuse", diffuse);
    shader.set4Float(name + ".specular", specular);
}

// render spot light into shader
void SpotLight::render(Shader shader, int idx) {
    // get name with index in array
    std::string name = "spotLights[" + std::to_string(idx) + "]";

    // set position
    shader.set3Float(name + ".position", position);
    // set direction
    shader.set3Float(name + ".direction", direction);

    // set first level cut off
    shader.setFloat(name + ".cutOff", cutOff);
    // set second level cut off
    shader.setFloat(name + ".outerCutOff", outerCutOff);

    // set attenuation constants
    shader.setFloat(name + ".k0", k0);
    shader.setFloat(name + ".k1", k1);
    shader.setFloat(name + ".k2", k2);

    // set lighting values
    shader.set4Float(name + ".ambient", ambient);
    shader.set4Float(name + ".diffuse", diffuse);
    shader.set4Float(name + ".specular", specular);
}