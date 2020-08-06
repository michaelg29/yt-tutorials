#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <stb/stb_image.h>

/*
    class to represent texture
*/

class Texture {
public:
    /*
        constructor
    */

    // initialize with image path and type
    Texture(std::string dir, std::string path, aiTextureType type);

    // generate texture id
    void generate();

    // load texture from path
    void load(bool flip = true);

    // bind texture id
    void bind();

    /*
        texture object values
    */
    
    // texture id
    unsigned int id;
    // texture type
    aiTextureType type;
    // directory of image
    std::string dir;
    // name of image
    std::string path;
};

#endif