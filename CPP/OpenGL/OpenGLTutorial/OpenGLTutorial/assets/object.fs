#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// uniform sampler2D texture0;
// uniform sampler2D texture1;

uniform float mixVal;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main() {
	// FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), mixVal);

	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = light.specular * (spec * material.specular);

	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
}