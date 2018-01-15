#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	//sampler2D emission;
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
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define MAX_POINT_LIGHTS 16
uniform PointLight pointLights[MAX_POINT_LIGHTS];

struct SpotLight {
	float intensity;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutOff;
	float outerCutOff;
};

uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 objectColor;

vec3 calcDirLight(DirLight light, vec3 normal) {

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	ambient *= light.intensity;

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords));
	diffuse *= light.intensity;

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * vec3(texture(material.specular, TexCoords));
	specular *= light.intensity;

	return (ambient + diffuse + specular) * light.color;

}

vec3 calcPointLight(PointLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	ambient *= attenuation * light.intensity;

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords));
	diffuse *= attenuation * light.intensity;

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * vec3(texture(material.specular, TexCoords));
	specular *= attenuation * light.intensity;

	return (ambient + diffuse + specular);

}

vec3 calcSpotLight(SpotLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	ambient *= attenuation * light.intensity;

	vec3 result;

	if(theta > light.outerCutOff) {

		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
		diffuse *= attenuation * intensity;

		vec3 reflectDir = reflect(-lightDir, norm);
		vec3 viewDir = normalize(-FragPos);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
		specular *= attenuation * intensity;

		return ambient + diffuse + specular;

	} else {

		return ambient;

	}
}

void main() {

	vec3 norm = normalize(Normal);

	vec3 result;

	for(int i = 0; i < dirLightCount; i++) {
		result += calcDirLight(dirLights[i], norm);
	}

	//for(int i = 0; i < NR_POINT_LIGHTS; i++) {
	//	result += calcPointLight(pointLights[i], norm);
	//}

	//result += calcSpotLight(spotLight, norm);

	FragColor = vec4(result, 1.0);
}