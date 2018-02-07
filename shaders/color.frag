#version 330 core

in vData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	mat4 InvView;
} fs_in;

out vec4 FragColor;

struct Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 mirrorColor;
	sampler2D diffuse;
	sampler2D specular;
	bool hasDiffuse;
	bool hasSpecular;
	float shininess;
	float reflectivity;
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
uniform vec3 bgColor;
uniform bool skyboxEnabled;

vec3 calcDirLightDiffuse(DirLight light, vec3 normal) {

	vec3 matDiff;
	if(material.hasDiffuse) {
		matDiff = vec3(texture(material.diffuse, fs_in.TexCoords));
	} else {
		matDiff = material.diffuseColor;
	}
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * matDiff;

	vec3 ambient = light.ambient * matDiff;

	return (ambient + diffuse) * light.color * light.intensity;

}

vec3 calcDirLightSpecular(DirLight light, vec3 normal) {

	vec3 lightDir = normalize(-light.direction);
	vec3 viewDir = normalize(-fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, fs_in.TexCoords));
	} else {
		matSpec = material.specularColor;
	}
	vec3 specular = spec * matSpec;

	return specular * light.color * light.intensity;

}

vec3 calcPointLightDiffuse(PointLight light, vec3 normal) {

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 matDiff;
	if(material.hasDiffuse) {
		matDiff = vec3(texture(material.diffuse, fs_in.TexCoords));
	} else {
		matDiff = material.diffuseColor;
	}
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * matDiff;

	vec3 ambient = light.ambient * matDiff;

	return (ambient + diffuse) * light.color * light.intensity * attenuation;

}

vec3 calcPointLightSpecular(PointLight light, vec3 normal) {

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	vec3 viewDir = normalize(-fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 matSpec;
	if(material.hasSpecular) {
		matSpec = vec3(texture(material.specular, fs_in.TexCoords));
	} else {
		matSpec = material.specularColor;
	}
	vec3 specular = spec * matSpec;

	return specular * light.color * light.intensity * attenuation;

}

vec3 calcSpotLightDiffuse(SpotLight light, vec3 normal) {

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 matDiff;
	if(material.hasDiffuse) {
		matDiff = vec3(texture(material.diffuse, fs_in.TexCoords));
	} else {
		matDiff = material.diffuseColor;
	}

	vec3 ambient = light.ambient * matDiff;

	if(theta > light.outerCutOff) {

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * matDiff;

		vec3 multiplier = light.color * light.intensity * attenuation;
		return (ambient + diffuse * intensity) * multiplier;

	} else {

		return ambient * light.intensity * attenuation;

	}
}

vec3 calcSpotLightSpecular(SpotLight light, vec3 normal) {

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));

	if(theta > light.outerCutOff) {

		vec3 viewDir = normalize(-fs_in.FragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
		vec3 matSpec;
		if(material.hasSpecular) {
			matSpec = vec3(texture(material.specular, fs_in.TexCoords));
		} else {
			matSpec = material.specularColor;
		}
		vec3 specular = spec * matSpec;

		vec3 multiplier = light.color * light.intensity * attenuation;
		return specular * intensity * multiplier;

	} else {

		return vec3(0.0, 0.0, 0.0);

	}
}

void main() {

	float opacity;
	if(material.hasDiffuse) {
		opacity = texture(material.diffuse, fs_in.TexCoords).a;
		if(opacity == 0.0) {
			discard;
		}
	} else {
		opacity = 1.0;
	}
	
	vec3 norm = normalize(fs_in.Normal);

	vec3 resultDiffuse = vec3(0.0, 0.0, 0.0);
	vec3 resultSpecular = vec3(0.0, 0.0, 0.0);

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

	vec3 I = normalize(fs_in.FragPos);
	vec3 R = reflect(I, norm);
	vec3 reflectColor;
	if(skyboxEnabled) {
		reflectColor = texture(skybox, vec3(fs_in.InvView * vec4(R, 0.0))).rgb * material.mirrorColor;
	} else {
		reflectColor = bgColor * material.mirrorColor;
	}
	vec3 result = mix(resultDiffuse, reflectColor, material.reflectivity) + resultSpecular;

	FragColor = vec4(result, opacity);

	//FragColor = vec4(fs_in.TexCoords, 0.0, opacity);

/*
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(FragPos);
	vec3 R = refract(I, norm, ratio);
	vec3 refractColor = texture(skybox, invView * R).rgb;

	FragColor = vec4(refractColor, texture(material.diffuse, TexCoords).a);
*/

}