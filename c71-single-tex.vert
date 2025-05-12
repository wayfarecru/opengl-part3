#version 330 core

in vec4 aPos; // vertex position: attribute
in vec2 aTexCoord; // texture coordinate 2D: attribute
out vec2 vTexCoord; // texture coordinate 2D: varying

void main(void) {
	gl_Position = aPos; // transformation
	gl_Position.z *= -1.0F; // negation for depth processing, if needed
	vTexCoord = aTexCoord;
}
