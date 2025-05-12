#version 330 core

in vec4 aPos;
in vec2 aTexCoord;
out vec2 vTexCoord;

uniform mat4 uMat;

void main(void) {
	gl_Position = uMat * aPos;
	vTexCoord = aTexCoord;
}
