#version 330 core

in vec2 vTexCoordRose; // texture coordinate 2D : varing
in vec2 vTexCoordLogo; // texture coordinate 2D : varing
uniform sampler2D texSamplerRose; // texture sampler 2D
uniform sampler2D texSamplerLogo; // texture sampler 2D
out vec4 FragColor; // fragment color: framebuffer
uniform float alpha; // alpha blending factor

void main(void) {
	vec4 colorDst = texture( texSamplerRose, vTexCoordRose );
	vec4 colorSrc = texture( texSamplerLogo, vTexCoordLogo );
	FragColor = alpha * colorSrc + (1.0 - alpha) * colorDst;
}
