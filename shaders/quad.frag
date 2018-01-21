#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

const float offset = 1.0 / 1000.0;

void main() {

	//original
	FragColor = texture(screenTexture, TexCoords);

	//negative
	//FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

	//grayscale
	/*
	vec4 origColor = texture(screenTexture, TexCoords);
	float average = 0.2126 * origColor.r + 0.7152 * origColor.g + 0.0722 * origColor.b;
	FragColor = vec4(vec3(average), 1.0);
	*/

	//kernel
	/*
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0),
		vec2(0.0, 0.0),
		vec2(offset, 0.0),
		vec2(-offset, -offset),
		vec2(0.0, -offset),
		vec2(offset, -offset)
	);

	float kernel[9] = float[](
		1,  1, 1,
		1, -8, 1,
		1,  1, 1
	);
	for(int i = 0; i < 9; i++) {
		kernel[i] /= 1.0;
	}

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++) {
		col += sampleTex[i] * kernel[i];
	}
	FragColor = vec4(col, 1.0);
	*/

}