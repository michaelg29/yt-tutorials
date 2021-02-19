#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aOffset;
layout (location = 5) in vec3 aSize;

void main() {
	gl_Position = vec4(aSize * aPos + aOffset, 1.0);
}