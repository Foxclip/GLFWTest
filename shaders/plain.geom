#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	mat4 InvView;
} gs_in[];

out	vec2 TexCoords;
out	vec3 Normal;
out vec3 FragPos;
out mat4 InvView;

void main() {
	for(int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		Normal = gs_in[i].Normal;
		TexCoords = gs_in[i].TexCoords;
		FragPos = gs_in[i].FragPos;
		InvView = gs_in[i].InvView;
		EmitVertex();
	}
	EndPrimitive();
}