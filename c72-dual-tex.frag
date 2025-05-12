#version 330 core

in vec2 vTexCoordRose; // texture coordinate 2D : varing
in vec2 vTexCoordLogo; // texture coordinate 2D : varing
uniform sampler2D texSamplerRose; // texture sampler 2D
uniform sampler2D texSamplerLogo; // texture sampler 2D
out vec4 FragColor; // fragment color: framebuffer

void main(void) {
	vec4 color = texture( texSamplerRose, vTexCoordRose );
	color *= texture( texSamplerLogo, vTexCoordLogo );
	FragColor = color;
}
