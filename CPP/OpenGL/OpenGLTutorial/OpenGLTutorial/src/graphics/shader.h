#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
    class to represent shader program
*/

class Shader {
public:
    // program ID
    unsigned int id;

    /*
        constructors
    */

    // default
    Shader();

    // initialize with paths to vertex, fragment, and optional geometry shaders
    Shader(const char* vertexShaderPath, 
        const char* fragShaderPath, 
        const char* geoShaderPath = nullptr);

    /*
        process functions
    */

    // generate using vertex, frag, and optional geometry shaders
    void generate(const char* vertexShaderPath, 
        const char* fragShaderPath, 
        const char* geoShaderPath = nullptr);

    // activate shader
    void activate();

    /*
        utility functions
    */

    // load string from file
    std::string loadShaderSrc(const char* filePath);

    // compile shader program
    GLuint compileShader(const char* filePath, GLuint type);

    /*
        set uniform variables
    */

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void set3Float(const std::string& name, float v1, float v2, float v3);
    void set3Float(const std::string& name, glm::vec3 v);
    void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
    void set4Float(const std::string& name, aiColor4D color);
    void set4Float(const std::string& name, glm::vec4 v);
    void setMat4(const std::string& name, glm::mat4 val);
};

#endif