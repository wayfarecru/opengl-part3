#version 330 core

in vec2 vTexCoord; // texture coordinate 2D : varing
uniform sampler2D texSampler; // texture sampler 2D
out vec4 FragColor; // fragment color: framebuffer

void main(void) {
	FragColor = texture( texSampler, vTexCoord );
}
