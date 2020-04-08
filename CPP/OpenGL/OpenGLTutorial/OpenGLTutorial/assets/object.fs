#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixVal;

void main() {
	FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), mixVal);
}