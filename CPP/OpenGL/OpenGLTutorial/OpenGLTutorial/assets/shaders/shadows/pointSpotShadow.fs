#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
	// get distance between the fragment and the light position
	float lightDist = length(FragPos.xyz - lightPos);

	// map to [0, 1]
	lightDist /= farPlane;

	// write to depth map
	gl_FragDepth = lightDist;
}