#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	mat4 InvView;
} gs_in[];

out vData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	mat4 InvView;
} gs_out;

void main() {
	for(int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		gs_out.Normal = gs_in[i].Normal;
		gs_out.TexCoords = gs_in[i].TexCoords;
		gs_out.FragPos = gs_in[i].FragPos;
		gs_out.InvView = gs_in[i].InvView;
		EmitVertex();
	}
	EndPrimitive();
}