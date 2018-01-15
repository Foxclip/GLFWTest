#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
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

	vec3 emission = vec3(texture(material.emission, TexCoords));

	vec3 lightDir = normalize(light.position - FragPos);
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(-FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec3 result = emission + ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}