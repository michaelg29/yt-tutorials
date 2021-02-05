struct Material {
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform bool noTex;
uniform bool noNormalMap;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;

uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 20
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

#define MAX_SPOT_LIGHTS 5
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
} fs_in;

uniform Material material;

uniform bool skipNormalMapping;
uniform vec3 viewPos;

vec4 calcDirLight(vec3 norm, vec3 viewVec, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewVec, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewVec, vec3 viewDir, vec4 diffMap, vec4 specMap);

#define NUM_SAMPLES 20
#define NUM_SAMPLESF 20.0
vec3 sampleOffsetDirections[NUM_SAMPLES] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  0,  0)
);

void main() {
	// properties
	vec3 norm = normalize(fs_in.Normal);

	if (!skipNormalMapping && !noNormalMap) {
		norm = texture(normal0, fs_in.TexCoord).rgb;
		norm = normalize(norm * 2.0 - 1.0); // map from [0, 1] to [-1, 1]
	}

	vec3 viewVec = viewPos - fs_in.FragPos;
	vec3 viewDir = normalize(viewVec);

	vec4 diffMap;
	vec4 specMap;

	if (noTex) {
		diffMap = material.diffuse;
		specMap = material.specular;
	} else {
		diffMap = texture(diffuse0, fs_in.TexCoord);
		specMap = texture(specular0, fs_in.TexCoord);
	}

	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

	// directional
	//result += calcDirLight(norm, viewVec, viewDir, diffMap, specMap);

	// point lights
	for (int i = 0; i < noPointLights; i++) {
		result += calcPointLight(i, norm, viewVec, viewDir, diffMap, specMap);
	}

	// spot lights
	for (int i = 0; i < noSpotLights; i++) {
		result += calcSpotLight(i, norm, viewVec, viewDir, diffMap, specMap);
	}

	// gamma correction
	float gamma = 2.2;
	result.rgb = pow(result.rgb, vec3(1.0 / gamma));

	// depth testing
	float near = 0.1;
	float far = 100.0;
	float z = gl_FragCoord.z * 2.0 - 1.0; // transform to NDC [0, 1] => [-1, 1]
	float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]

	result.rgb *= 1 - factor;

	FragColor = result;
}

float calcDirLightShadow(vec3 norm, vec3 viewVec, vec3 lightDir) {
	vec4 fragPosLightSpace = dirLight.lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);

	// perspective divide (transforming coordinates NDC)
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // [depth relative to light] => [-1, 1]

	// NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1, 1] => [0, 1]

	// if too far from light, do not return any shadow
	if (projCoords.z > 1.0) {
		return 0.0;
	}

	// get depth of fragment
	float currentDepth = projCoords.z;

	// calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm, lightDir)), minBias);

	// PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirLight.depthBuffer, 0);
	float viewDist = length(viewVec);
	float diskRadius = (1.0 + (viewDist / dirLight.farPlane)) / 30.0;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			float pcfDepth = texture(dirLight.depthBuffer,
				projCoords.xy + vec2(x, y) * texelSize * diskRadius).r;
			shadowSum += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	// return average
	return shadowSum / 9.0;
}

vec4 calcDirLight(vec3 norm, vec3 viewDir, vec3 viewVec, vec4 diffMap, vec4 specMap) {
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
		// calculate using Blinn-Phong model
		vec3 halfwayDir = normalize(lightDir + viewDir);
		dotProd = dot(norm, halfwayDir);

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	//float shadow = calcDirLightShadow(norm, viewVec, lightDir);
	float shadow = 0.0;

	return vec4(ambient + (1.0 - shadow) * (diffuse + specular));
}

float calcPointLightShadow(int idx, vec3 norm, vec3 viewVec, vec3 lightDir) {
	// get vector from the light to the fragment
	vec3 lightToFrag = fs_in.FragPos - pointLights[idx].position;

	// get depth from cubemap
	float closestDepth = texture(pointLights[idx].depthBuffer, lightToFrag).r;

	// [0, 1] => original depth value
	closestDepth *= pointLights[idx].farPlane;

	// get current depth
	float currentDepth = length(lightToFrag);

	// calculate bias
	float minBias = 0.005;
	float maxBias = 0.05;
	float bias = max(maxBias * (1.0 - dot(norm, lightDir)), minBias);

	// PCF
	float shadow = 0.0;
	float viewDist = length(viewVec);
	float diskRadius = (1.0 + (viewDist / pointLights[idx].farPlane)) / 30.0;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float pcfDepth = texture(pointLights[idx].depthBuffer, 
			lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
		pcfDepth *= pointLights[idx].farPlane;

		if (currentDepth - bias > pcfDepth) {
			shadow += 1.0;
		}
	}

	shadow /= NUM_SAMPLESF;

	return shadow;
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewVec, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	// ambient
	vec4 ambient = pointLights[idx].ambient * diffMap;

	// diffuse
	vec3 lightDir = normalize(pointLights[idx].position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = pointLights[idx].diffuse * (diff * diffMap);

	// specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if (diff > 0) {
		// if diff <= 0, object is "behind" light

		float dotProd = 0.0;
		// calculate using Blinn-Phong model
		vec3 halfwayDir = normalize(lightDir + viewDir);
		dotProd = dot(norm, halfwayDir);

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	// calculate attenuation
	float dist = length(pointLights[idx].position - fs_in.FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist));

	// apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//float shadow = calcPointLightShadow(idx, norm, viewVec, lightDir);
	float shadow = 0.0;

	return vec4(ambient + (1.0 - shadow) * (diffuse + specular));
}

float calcSpotLightShadow(int idx, vec3 norm, vec3 viewVec, vec3 lightDir) {
	vec4 fragPosLightSpace = spotLights[idx].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);

	// perspective divide (transform to NDC)
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // [near, far] => [-1, 1]

	// NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1, 1] => [0, 1]

	// if too far from light, do not return any shadow
	if (projCoords.z > 1.0) {
		return 0.0;
	}

	// get depth of fragment
	float currentDepth = projCoords.z;

	// calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm, lightDir)), minBias);

	// PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(spotLights[idx].depthBuffer, 0);
	float viewDist = length(viewVec);
	float diskRadius = (1.0 + (viewDist / spotLights[idx].farPlane)) / 30.0;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			float pcfDepth = texture(spotLights[idx].depthBuffer,
				projCoords.xy + vec2(x, y) * texelSize * diskRadius).r;
			pcfDepth *= spotLights[idx].farPlane;

			shadowSum += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	// return average
	return shadowSum / 9.0;
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewVec, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	vec3 lightDir = normalize(spotLights[idx].position - fs_in.FragPos);
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
			// calculate using Blinn-Phong model
			vec3 halfwayDir = normalize(lightDir + viewDir);
			dotProd = dot(norm, halfwayDir);

			float spec = pow(max(dotProd, 0.0), material.shininess * 128);
			specular = dirLight.specular * (spec * specMap);
		}

		// calculate Intensity
		float intensity = clamp((theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff), 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		// calculate attenuation
		float dist = length(spotLights[idx].position - fs_in.FragPos);
		float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * (dist * dist));

		// apply attenuation
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		//float shadow = calcSpotLightShadow(idx, norm, viewVec, lightDir);
		float shadow = 0.0;

		return vec4(ambient + (1.0 - shadow) * (diffuse + specular));
	}
	else {
		// render just ambient
		return ambient;
	}
}