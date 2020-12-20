#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct DirLight {
	sampler2D depthBuffer;
};
uniform DirLight dirLight;

uniform sampler2D bufferTex;

float near = 0.1;
float far = 100.0;

void main() {
	// DEPTH MAP (perspective)
	//float depth = texture(bufferTex, TexCoord).r;
	//float z = depth * 2.0 - 1.0; // transform to NDC [0, 1] => [-1, 1]
	//float linearDepth = (2.0 * near * far) / (depth * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	//float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]
	//FragColor = vec4(vec3(1 - factor), 1.0);

	// DEPTH MAP (orthographic)
	FragColor = vec4(vec3(texture(dirLight.depthBuffer, TexCoord).r), 1.0);

	// COLOR MAP
	//FragColor = texture(bufferTex, TexCoord);
}