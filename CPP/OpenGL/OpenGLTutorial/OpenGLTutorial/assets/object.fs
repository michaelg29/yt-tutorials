#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 20
struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float k0;
	float k1;
	float k2;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

#define MAX_SPOT_LIGHTS 5
struct SpotLight {
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float k0;
	float k1;
	float k2;
};
uniform SpotLight spotLights[5];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;

uniform vec3 viewPos;

vec3 calcDirLight(vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec);
vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec);
vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec);

void main() {
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 texDiff = vec3(texture(material.diffuse, TexCoord));
	vec3 texSpec = vec3(texture(material.specular, TexCoord));

	vec3 result;

	// directional
	result = calcDirLight(norm, viewDir, texDiff, texSpec);

	// point lights
	for (int i = 0; i < noPointLights; i++) {
		result += calcPointLight(i, norm, viewDir, texDiff, texSpec);
	}

	// spot lights
	for (int i = 0; i < noSpotLights; i++) {
		result += calcSpotLight(i, norm, viewDir, texDiff, texSpec);
	}

	FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec) {
	// ambient
	vec3 ambient = dirLight.ambient * texDiff;

	// diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * (diff * texDiff);

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = dirLight.specular * (spec * texSpec);

	return vec3(ambient + diffuse + specular);
}

vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec) {
	// ambient
	vec3 ambient = pointLights[idx].ambient * texDiff;

	// diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = pointLights[idx].diffuse * (diff * texDiff);

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = pointLights[idx].specular * (spec * texSpec);

	// calculate attenuation
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist));

	// apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec3(ambient + diffuse + specular);
}

vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 texDiff, vec3 texSpec) {
	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	// ambient
	vec3 ambient = spotLights[idx].ambient * texDiff;

	if (theta > spotLights[idx].outerCutOff) {
		// if in cutoff, render light
		// > because using cosines and not degrees

		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = spotLights[idx].diffuse * (diff * texDiff);

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec3 specular = spotLights[idx].specular * (spec * texSpec);

		// calculate Intensity
		float intensity = clamp((theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff), 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		// calculate attenuation
		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * (dist * dist));

		// apply attenuation
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		return vec3(ambient + diffuse + specular);
	}
	else {
		// render just ambient
		return ambient;
	}
}