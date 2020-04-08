#include "texture.h"

#include <iostream>

int Texture::currentId = 0;

Texture::Texture() {}

Texture::Texture(const char* path, const char* name, bool defaultParams) 
	: path(path), name(name), id(currentId++) {
	generate();

	if (defaultParams) {
		setFilters(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
		setWrap(GL_REPEAT);
	}
}

void Texture::generate() {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Texture::load(bool flip) {
	stbi_set_flip_vertically_on_load(flip);

	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);

	GLenum colorMode = GL_RGB;
	switch (nChannels) {
	case 1:
		colorMode = GL_RED;
		break;
	case 4:
		colorMode = GL_RGBA;
		break;
	};

	if (data) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Image not loaded at " << path << std::endl;
	}

	stbi_image_free(data);
}

void Texture::setFilters(GLenum all) {
	setFilters(all, all);
}

void Texture::setFilters(GLenum mag, GLenum min) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
}

void Texture::setWrap(GLenum all) {
	setWrap(all, all);
}

void Texture::setWrap(GLenum s, GLenum t) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}

void Texture::setBorderColor(float borderColor[4]) {
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void Texture::activate() {
	glActiveTexture(GL_TEXTURE0 + id);
}