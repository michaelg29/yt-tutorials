#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

class Texture {
public:
	Texture();
	Texture(const char* path, const char* name, bool defaultParams = true);

	void generate();
	void load(bool flip = true);

	void setFilters(GLenum all);
	void setFilters(GLenum mag, GLenum min);

	void setWrap(GLenum all);
	void setWrap(GLenum s, GLenum t);

	void setBorderColor(float borderColor[4]);

	void activate();

	// texture object
	int id;
	unsigned int tex;
	const char* name;

private:
	static int currentId;

	// img properties
	const char* path;
	int width;
	int height;
	int nChannels;
};

#endif