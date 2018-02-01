#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vData {
	vec2 TexCoords;
	vec3 Normal;
	vec4 ClipNormal;
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

void main() {
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal = newNormal * aNormal; //view space normals
	vs_out.ClipNormal = projection * vec4(vs_out.Normal, 0.0); //clip space normals
	vs_out.FragPos = vec3(view * model * vec4(aPos, 1.0)); //view space positions
	vs_out.InvView = invView; //inverse view matrix
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}