#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat4 InvView;

uniform mat4 model;
layout (std140) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 invView;
};
uniform mat3 newNormal;

void main() {
	Normal = newNormal * aNormal;
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	TexCoords = aTexCoords;
	InvView = invView;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}