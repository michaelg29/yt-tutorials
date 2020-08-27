#version 330 core
out vec4 FragColor;

uniform float time;

uniform vec3 min;
uniform vec3 max;

void main() {
	/*
		sinusoid:
		color[i](t) = Asin(ft - h) + k

		A = amplitude = (max - min) / 2
		f = frequency; 2pi/T = f; T = 24; f = 2pi / 24;
		h = horizontal shift (in radians) = pi / 2
		k = vertical shift = average of min/max = (min + max) / 24
		t = time
	*/

	float pi = atan(1.0) * 4;

	for (int i = 0; i < 3; i++) {
		FragColor[i] = ((max[i] - min[i]) / 2) * sin((2 * pi / 24) * time - (pi / 2)) + ((min[i] + max[i]) / 2);
	}

	FragColor[3] = 1.0;
}