layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 model;
layout (location = 8) in mat3 normalModel;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoord;

	TangentLights tanLights;
} vs_out;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main() {
	// get position in world space
	// apply model transformation
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

	// set texture coordinate
	vs_out.TexCoord = aTexCoord;

	// determine normal vector in tangent space
	vs_out.tanLights.Normal = normalize(normalModel * aNormal);

	// calculate tangent space matrix
	vec3 T = normalize(normalModel * aTangent);
	vec3 N = vs_out.tanLights.Normal;
	T = normalize(T - dot(T, N) * N); // re-orthogonalize T with respect to N
	vec3 B = cross(N, T); // get B, perpendicular to N and T
	mat3 TBNinv = transpose(mat3(T, B, N)); // orthogonal matrix => transpose = inverse

	// transform positions to the tangent space
	vs_out.tanLights.FragPos = TBNinv * vs_out.FragPos;
	vs_out.tanLights.viewPos = TBNinv * viewPos;

	// directional light
	vs_out.tanLights.dirLightDirection = TBNinv * dirLight.direction;

	// point lights
	for (int i = 0; i < noPointLights; i++) {
		vs_out.tanLights.pointLightPositions[i] = TBNinv * pointLights[i].position;
	}

	// spot lights
	for (int i = 0; i < noSpotLights; i++) {
		vs_out.tanLights.spotLightPositions[i] = TBNinv * spotLights[i].position;
		vs_out.tanLights.spotLightDirections[i] = TBNinv * spotLights[i].direction;
	}

	// set output for fragment shader
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}