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

vec4 calcDirLight(DirLight light, vec3 normal) {

	vec4 ambient = vec4(light.ambient, 1.0) * texture(material.diffuse, TexCoords);

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec4 diffuse = diff * texture(material.diffuse, TexCoords);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, TexCoords));
	} else {
		matSpec = vec3(1.0);
	}
	vec4 specular = spec * vec4(matSpec, 1.0);

	return (ambient + diffuse + specular) * vec4(light.color, 1.0) * light.intensity;

}

vec4 calcPointLight(PointLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec4 ambient = vec4(light.ambient, 1.0) * texture(material.diffuse, TexCoords);

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec4 diffuse = diff * texture(material.diffuse, TexCoords);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, TexCoords));
	} else {
		matSpec = vec3(1.0);
	}
	vec4 specular = spec * vec4(matSpec, 1.0);

	return (ambient + diffuse + specular) * vec4(light.color, 1.0) * light.intensity * attenuation;

}

vec4 calcSpotLight(SpotLight light, vec3 normal) {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec4 ambient = vec4(light.ambient, 1.0) * texture(material.diffuse, TexCoords);

	if(theta > light.outerCutOff) {

		float diff = max(dot(normal, lightDir), 0.0);
		vec4 diffuse = diff * texture(material.diffuse, TexCoords);

		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 viewDir = normalize(-FragPos);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 matSpec;
		if(material.hasSpecular) {
			matSpec = vec3(texture(material.specular, TexCoords));
		} else {
			matSpec = vec3(1.0);
		}
		vec4 specular = spec * vec4(matSpec, 1.0);

		vec4 multiplier = vec4(light.color, 1.0) * light.intensity * attenuation;
		return (ambient + (diffuse + specular)*intensity) * multiplier;

	} else {

		return ambient * light.intensity * attenuation;

	}
}

void main() {

	vec4 diffuseColor = texture(material.diffuse, TexCoords);
	if(diffuseColor.a < 0.1) {
		discard;
	}

	vec3 norm = normalize(Normal);

	vec4 result;

	for(int i = 0; i < dirLightCount; i++) {
		result += calcDirLight(dirLights[i], norm);
	}

	for(int i = 0; i < pointLightCount; i++) {
		result += calcPointLight(pointLights[i], norm);
	}

	for(int i = 0; i < spotLightCount; i++) {
		result += calcSpotLight(spotLights[i], norm);
	}

	FragColor = result;
}