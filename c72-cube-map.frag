#version 330 core

in vec3 vTexCoord; // texture coordinate cubemap : varing
uniform samplerCube texSampler; // texture sampler cubemap
out vec4 FragColor; // fragment color: framebuffer

void main(void) {
	FragColor = texture( texSampler, vTexCoord );
}
