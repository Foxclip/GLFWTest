#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vData {
	vec2 TexCoords;
	vec3 Normal;
	vec4 ClipNormal;
	vec3 FragPos;	
	mat4 InvView;
} gs_in[];

out	vec3 Normal;
out vec3 FragPos;

const float MAGNITUDE = 0.4;

void GenerateLine(int index) {
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + gs_in[index].ClipNormal * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}

void main() {
	/*
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
	*/
	vec4 faceNormal = normalize(gs_in[0].ClipNormal + gs_in[1].ClipNormal + gs_in[2].ClipNormal);
	vec4 faceCenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
	gl_Position = faceCenter;
	EmitVertex();
	gl_Position = faceCenter + faceNormal * MAGNITUDE;
	EmitVertex();
	EndPrimitive();
}