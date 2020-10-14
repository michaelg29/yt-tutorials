#version 330 core
struct Material {
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform int noTex;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

struct DirLight {
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 20
struct PointLight {
	vec3 position;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

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

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float k0;
	float k1;
	float k2;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;

uniform vec3 viewPos;
uniform bool useBlinn;
uniform bool useGamma;

vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);

void main() {
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec4 diffMap;
	vec4 specMap;

	if (noTex == 1) {
		diffMap = material.diffuse;
		specMap = material.specular;
	} else {
		diffMap = texture(diffuse0, TexCoord);
		specMap = texture(specular0, TexCoord);
	}

	vec4 result;

	// directional
	result = calcDirLight(norm, viewDir, diffMap, specMap);

	// point lights
	for (int i = 0; i < noPointLights; i++) {
		result += calcPointLight(i, norm, viewDir, diffMap, specMap);
	}

	// spot lights
	for (int i = 0; i < noSpotLights; i++) {
		result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
	}

	if (useGamma) {
		float gamma = 2.2;
		result.rgb = pow(result.rgb, vec3(1.0 / gamma));
	}

	FragColor = result;
}

vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	// ambient
	vec4 ambient = dirLight.ambient * diffMap;

	// diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = dirLight.diffuse * (diff * diffMap);

	// specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if (diff > 0) {
		// if diff <= 0, object is "behind" light

		float dotProd = 0.0;
		if (useBlinn) {
			// calculate using Blinn-Phong model
			vec3 halfwayDir = normalize(lightDir + viewDir);
			dotProd = dot(norm, halfwayDir);
		}
		else {
			// calculate using Phong model
			vec3 reflectDir = reflect(-lightDir, norm);
			dotProd = dot(viewDir, reflectDir);
		}

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	return vec4(ambient + diffuse + specular);
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	// ambient
	vec4 ambient = pointLights[idx].ambient * diffMap;

	// diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = pointLights[idx].diffuse * (diff * diffMap);

	// specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if (diff > 0) {
		// if diff <= 0, object is "behind" light

		float dotProd = 0.0;
		if (useBlinn) {
			// calculate using Blinn-Phong model
			vec3 halfwayDir = normalize(lightDir + viewDir);
			dotProd = dot(norm, halfwayDir);
		}
		else {
			// calculate using Phong model
			vec3 reflectDir = reflect(-lightDir, norm);
			dotProd = dot(viewDir, reflectDir);
		}

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	// calculate attenuation
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist));

	// apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4(ambient + diffuse + specular);
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	// ambient
	vec4 ambient = spotLights[idx].ambient * diffMap;

	if (theta > spotLights[idx].outerCutOff) {
		// if in cutoff, render light
		// > because using cosines and not degrees

		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec4 diffuse = spotLights[idx].diffuse * (diff * diffMap);

		// specular
		vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
		if (diff > 0) {
			// if diff <= 0, object is "behind" light

			float dotProd = 0.0;
			if (useBlinn) {
				// calculate using Blinn-Phong model
				vec3 halfwayDir = normalize(lightDir + viewDir);
				dotProd = dot(norm, halfwayDir);
			}
			else {
				// calculate using Phong model
				vec3 reflectDir = reflect(-lightDir, norm);
				dotProd = dot(viewDir, reflectDir);
			}

			float spec = pow(max(dotProd, 0.0), material.shininess * 128);
			specular = dirLight.specular * (spec * specMap);
		}

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

		return vec4(ambient + diffuse + specular);
	}
	else {
		// render just ambient
		return ambient;
	}
}