#version 330 core

in vec4 aPos; // vertex position: attribute
in vec2 aTexCoordRose; // texture coordinate 2D: attribute
in vec2 aTexCoordLogo; // texture coordinate 2D: attribute
out vec2 vTexCoordRose; // texture coordinate 2D: varying
out vec2 vTexCoordLogo; // texture coordinate 2D: varying

void main(void) {
	gl_Position = aPos; // transformation
	gl_Position.z *= -1.0F; // negation for depth processing, if needed
	vTexCoordRose = aTexCoordRose;
	vTexCoordLogo = aTexCoordLogo;
}
