#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;	
	mat4 InvView;
} vs_out;

layout (std140) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 invView;
};
uniform mat4 model;
uniform mat3 newNormal;
uniform mat4 parentMatrix;

uniform bool isInstance;

void main() {
	mat4 modelMatrix;
	vec3 normal;
	if(isInstance) {
		modelMatrix = parentMatrix * instanceMatrix;
		normal = mat3(transpose(inverse(view * modelMatrix))) * aNormal;
	} else {
		modelMatrix = model;
		normal = newNormal * aNormal;
	}
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal = normal;
	vs_out.FragPos = vec3(view * modelMatrix * vec4(aPos, 1.0)); //view space positions
	vs_out.InvView = invView;
	gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
}