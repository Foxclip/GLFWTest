#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

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

uniform Material material;
uniform Light light;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {

	vec3 lightDir = normalize(light.position - FragPos);
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = material.diffuse * light.diffuse * diff;

	vec3 ambient = material.ambient * light.ambient;

	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * light.specular * spec;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}