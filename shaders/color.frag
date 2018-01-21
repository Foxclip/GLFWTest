#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	//sampler2D emission;
	bool hasDiffuse;
	bool hasSpecular;
	float shininess;
};

struct DirLight {
	float intensity;
	vec3 color;
	vec3 direction;
	vec3 ambient;
};
#define MAX_DIRLIGHTS 16
uniform DirLight dirLights[MAX_DIRLIGHTS];
uniform int dirLightCount;

struct PointLight {
	float intensity;
	vec3 color;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
};
#define MAX_POINT_LIGHTS 16
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

struct SpotLight {
	float intensity;
	vec3 color;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	float cutOff;
	float outerCutOff;
};
#define MAX_SPOT_LIGHTS 16
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spotLightCount;

uniform SpotLight spotLight;
uniform Material material;

uniform samplerCube skybox;
uniform mat3 invView;

vec3 calcDirLightDiffuse(DirLight light, vec3 normal) {

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords));

	return (ambient + diffuse) * light.color * light.intensity;

}

vec3 calcDirLightSpecular(DirLight light, vec3 normal) {

	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, TexCoords));
	} else {
		matSpec = vec3(1.0);
	}
	vec3 specular = spec * matSpec;

	return specular * light.color * light.intensity;

}

vec3 calcPointLightDiffuse(PointLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords));

	return (ambient + diffuse) * light.color * light.intensity * attenuation;

}

vec3 calcPointLightSpecular(PointLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, TexCoords));
	} else {
		matSpec = vec3(1.0);
	}
	vec3 specular = spec * matSpec;

	return specular * light.color * light.intensity * attenuation;

}

vec3 calcSpotLightDiffuse(SpotLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	if(theta > light.outerCutOff) {

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords));

		vec3 multiplier = light.color * light.intensity * attenuation;
		return (ambient + diffuse * intensity) * multiplier;

	} else {

		return ambient * light.intensity * attenuation;

	}
}

vec3 calcSpotLightSpecular(SpotLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	if(theta > light.outerCutOff) {

		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 viewDir = normalize(-FragPos);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 matSpec;
		if(material.hasSpecular) {
			matSpec = vec3(texture(material.specular, TexCoords));
		} else {
			matSpec = vec3(1.0);
		}
		vec3 specular = spec * matSpec;

		vec3 multiplier = light.color * light.intensity * attenuation;
		return specular * intensity * multiplier;

	}
}

void main() {

	vec3 norm = normalize(Normal);

	vec3 resultDiffuse;
	vec3 resultSpecular;

	for(int i = 0; i < dirLightCount; i++) {
		resultDiffuse += calcDirLightDiffuse(dirLights[i], norm);
		resultSpecular += calcDirLightSpecular(dirLights[i], norm);
	}
	for(int i = 0; i < pointLightCount; i++) {
		resultDiffuse += calcPointLightDiffuse(pointLights[i], norm);
		resultSpecular += calcPointLightSpecular(pointLights[i], norm);
	}
	for(int i = 0; i < spotLightCount; i++) {
		resultDiffuse += calcSpotLightDiffuse(spotLights[i], norm);
		resultSpecular += calcSpotLightSpecular(spotLights[i], norm);
	}

	vec3 I = normalize(FragPos);
	vec3 R = reflect(I, norm);
	vec3 reflectColor = texture(skybox, invView * R).rgb;
	vec3 result = mix(resultDiffuse, reflectColor, 0.5) + resultSpecular;

	FragColor = vec4(result, texture(material.diffuse, TexCoords).a);

}