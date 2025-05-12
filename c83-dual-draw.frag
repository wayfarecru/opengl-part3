#version 330 core

in vec4 vColor; // varying color: varing

layout (location = 0) out vec4 FragColor; // fragment color: framebuffer
layout (location = 1) out vec4 FragAnother; // fragment color: framebuffer

void main(void) {
	FragColor = vColor;
	float z = gl_FragCoord.z;
	FragAnother = vec4(z, z, z, 1.0);
}
