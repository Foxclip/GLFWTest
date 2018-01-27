#version 330 core

in vec4 FragPos;

out vec4 FragColor;

void main() {
	if(gl_FragCoord.x > 960) {
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	} else {
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
}