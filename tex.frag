#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord1;
in vec2 TexCoord2;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixFactor = 0.5;

void main() {
	FragColor = mix(texture(texture1, TexCoord1), texture(texture2, vec2(-TexCoord2.x, TexCoord2.y)), texture(texture2, vec2(-TexCoord2.x, TexCoord2.y)).a*mixFactor);
}