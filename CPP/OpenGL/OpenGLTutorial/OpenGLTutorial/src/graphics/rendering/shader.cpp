#include "Shader.h"

#include <stdio.h>
#include <fstream>

/*
    constructors
*/

// default
Shader::Shader() {}

// initialize with paths to vertex and fragment shaders
Shader::Shader(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragShaderPath, const char* geoShaderPath) {
    generate(includeDefaultHeader, vertexShaderPath, fragShaderPath, geoShaderPath);
}

/*
    process functions
*/

void compileAndAttach(GLuint id, bool includeDefaultHeader, const char* path, GLuint type) {
    if (!path) {
        return;
    }

    GLuint shader = Shader::compileShader(includeDefaultHeader, path, type);
    glAttachShader(id, shader);
    glDeleteShader(shader);
}

// generate using vertex and frag shaders
void Shader::generate(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragShaderPath, const char* geoShaderPath) {
    id = glCreateProgram();

    // compile and attach shaders
    compileAndAttach(id, includeDefaultHeader, vertexShaderPath, GL_VERTEX_SHADER);
    compileAndAttach(id, includeDefaultHeader, fragShaderPath, GL_FRAGMENT_SHADER);
    compileAndAttach(id, includeDefaultHeader, geoShaderPath, GL_GEOMETRY_SHADER);
    glLinkProgram(id);

    // linking errors
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char* infoLog = (char*)malloc(512);
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "Linking error:" << std::endl << infoLog << std::endl;
    }
}

// activate shader
void Shader::activate() {
    glUseProgram(id);
}

// cleanup
void Shader::cleanup() {
    glDeleteProgram(id);
}

/*
    set uniform variables
*/

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::set4Float(const std::string& name, aiColor4D color) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}

void Shader::set4Float(const std::string& name, glm::vec4 v) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setMat4(const std::string& name, glm::mat4 val) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

/*
    static
*/

// compile shader program
GLuint Shader::compileShader(bool includeDefaultHeader, const char* filePath, GLuint type) {
    // create shader from file
    GLuint ret = glCreateShader(type);
    GLchar* shader = loadShaderSrc(includeDefaultHeader, filePath);
    glShaderSource(ret, 1, &shader, NULL);
    glCompileShader(ret);
    free(shader);

    // catch compilation error
    int success;
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
    if (!success) {
        char* infoLog = (char*)malloc(512);
        glGetShaderInfoLog(ret, 512, NULL, infoLog);
        std::cout << "Error with shader comp." << filePath << ":" << std::endl << infoLog << std::endl;
    }

    return ret;
}

// stream containing default headers
std::stringstream Shader::defaultHeaders;

// load into default header
void Shader::loadIntoDefault(const char* filepath) {
    char *fileContents = Shader::loadShaderSrc(false, filepath);

    Shader::defaultHeaders << fileContents;

    free(fileContents);
}

// clear default header (after shader compilation)
void Shader::clearDefault() {
    Shader::defaultHeaders.clear();
}

// load string from file
char *Shader::loadShaderSrc(bool includeDefaultHeader, const char* filePath) {
    std::string fullPath = Shader::defaultDirectory + '/' + filePath;

    FILE* file = NULL;
    fopen_s(&file, fullPath.c_str(), "rb");
    if (!file) {
        std::cout << "Could not open " << filePath << std::endl;
        return NULL;
    }

    // move cursor to the end
    fseek(file, 0L, SEEK_END);
    // get length
    int len = ftell(file);
    // return to beginning
    fseek(file, 0, SEEK_SET);

    // read
    char* ret = NULL;
    int cursor = 0;
    if (includeDefaultHeader) {
        // copy header and advance cursor to read into space after default header
        cursor = Shader::defaultHeaders.str().size();
        ret = (char*)malloc(cursor + len + 1);
        memcpy_s(ret, cursor + len + 1, Shader::defaultHeaders.str().c_str(), cursor);
    }
    else {
        ret = (char*)malloc(len + 1);
    }
    // read from file
    fread(ret + cursor, 1, len, file);
    ret[cursor + len] = 0; // terminator

    return ret;
}