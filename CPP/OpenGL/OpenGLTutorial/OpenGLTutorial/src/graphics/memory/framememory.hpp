#ifndef FRAMEMEMORY_HPP
#define FRAMEMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "../rendering/texture.h"
#include "../rendering/cubemap.h"

class RenderbufferObject {
public:
    GLuint val;

    void generate() {
        glGenRenderbuffers(1, &val);
    }

    void bind() {
        glBindRenderbuffer(GL_RENDERBUFFER, val);
    }

    void allocate(GLenum format, GLuint width, GLuint height) {
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }

    void cleanup() {
        glDeleteRenderbuffers(1, &val);
    }
};

class FramebufferObject {
public:
    GLuint val;
    GLuint width;
    GLuint height;
    GLbitfield bitCombo;

    std::vector<GLuint> rbos;
    std::vector<Texture> textures;
    Cubemap cubemap;

    FramebufferObject()
        : val(0), width(0), height(0), bitCombo(0) {}

    FramebufferObject(GLuint width, GLuint height, GLbitfield bitCombo)
        : val(0), width(width), height(height), bitCombo(bitCombo) {}

    void generate() {
        glGenFramebuffers(1, &val);
    }

    void disableColorBuffer() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, val);
    }

    void setViewport() {
        glViewport(0, 0, width, height);
    }

    void clear() {
        glClear(bitCombo);
    }

    void activate() {
        bind();
        setViewport();
        clear();
    }

    void allocateAndAttachRBO(GLenum attachType, GLenum format) {
        GLuint rbo;

        // generate
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        // attach
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachType, GL_RENDERBUFFER, rbo);

        // add to list
        rbos.push_back(rbo);
    }

    void allocateAndAttachTexture(GLenum attachType, GLenum format, GLenum type) {
        std::string name = "tex" + textures.size();
        Texture tex(name);

        // allocate
        tex.bind();
        tex.allocate(format, width, height, type);
        Texture::setParams(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // attach
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);

        textures.push_back(tex);
    }

    void attachTexture(GLenum attachType, Texture tex) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);
    }

    void allocateAndAttachCubemap(GLenum attachType, GLenum format, GLenum type) {
        cubemap = Cubemap();

        cubemap.generate();
        cubemap.bind();
        cubemap.allocate(format, width, height, type);

        glFramebufferTexture(GL_FRAMEBUFFER, attachType, cubemap.id, 0);
    }

    void cleanup() {
        // delete RBOs
        glDeleteRenderbuffers(rbos.size(), &rbos[0]);

        // delete generated textures
        for (Texture t : textures) {
            t.cleanup();
        }

        // delete FBO
        glDeleteFramebuffers(1, &val);
    }
};

#endif