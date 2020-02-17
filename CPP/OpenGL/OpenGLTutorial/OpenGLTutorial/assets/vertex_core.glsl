#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor; // output color to frag shader

uniform mat4 transform; // set in code

void main() {
	gl_Position = transform * vec4(aPos, 1.0);
	ourColor = aColor;
}