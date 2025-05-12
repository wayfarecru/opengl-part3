#version 330 core

in vec4 vColor; // varying color: varing
out vec4 FragColor; // fragment color: framebuffer
uniform bool bHighlight; // if true, highlighted !

void main(void) {
	if (bHighlight == true) {
		FragColor = vec4(1.0, 1.0, 0.8, 1.0); // highlighted color
	} else {
		FragColor = vColor;
	}
}
