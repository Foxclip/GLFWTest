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

struct Light {

	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;
	float constant;
	float linear;
	float quadratic;

};

uniform Material material;
uniform Light light;
uniform vec3 objectColor;

void main() {

	float distance = length(light.position - FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	ambient *= attenuation;

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

		result = ambient + diffuse + specular;

	} else {

		result = ambient;

	}

	FragColor = vec4(result, 1.0);
}