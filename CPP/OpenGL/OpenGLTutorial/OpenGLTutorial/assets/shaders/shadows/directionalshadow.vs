#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aOffset;
layout (location = 4) in vec3 aSize;

uniform mat4 lightSpaceMatrix;

void main() {
	// ~ projection * view * model * pos
	gl_Position = lightSpaceMatrix * vec4(aSize * aPos + aOffset, 1.0);
}