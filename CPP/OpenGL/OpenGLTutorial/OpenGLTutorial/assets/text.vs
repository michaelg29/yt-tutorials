#version 330 core
layout (location = 0) in vec4 vertex; // posX, posY, texCoordX, texCoordY
out vec2 TexCoord;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoord = vertex.zw;
}