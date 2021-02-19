#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aOffset;
layout (location = 5) in vec3 aSize;

uniform mat4 lightSpaceMatrix;

out vec4 FragPos;

void main() {
	// ~ projection * view * model * pos
	FragPos = vec4(aSize * aPos + aOffset, 1.0);

	gl_Position = lightSpaceMatrix * FragPos;
}