#version 330 core

in vec4 vColor; // varying color: varing
out vec4 FragColor; // fragment color: framebuffer

void main(void) {
	float z = (gl_FragCoord.z - 0.35);
	float fog = 1.0 - 1.8 * z;
	vec4 src = vColor;
	vec4 dst = vec4(0.8, 0.8, 0.8, 1.0); // background color
	FragColor = fog * src + (1.0 - fog) * dst;
}
