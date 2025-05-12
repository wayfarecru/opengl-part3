#version 330 core

in vec4 aPos;
in vec3 aTexCoord;
out vec3 vTexCoord;

uniform mat4 uMat;

void main(void) {
	gl_Position = uMat * aPos;
	vTexCoord = aTexCoord;
}
