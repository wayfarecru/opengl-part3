#version 330

in vec4 aPos;
in vec4 aNorm;
in vec3 aTexCoord;
in vec3 aNormalCoord;
in vec3 aTangent;
out vec4 vPos;
out vec4 vNorm;
out vec3 vTexCoord;
out vec3 vNormalCoord;
out vec3 vTangent;

uniform mat4 uMat;

void main(void) {
	vPos = uMat * aPos; // position in view frame 
	vNorm = transpose(inverse(uMat)) * aNorm; // normal vector in view frame
	vTangent = mat3(uMat) * aTangent; // tangent vector in view frame
	vTexCoord = aTexCoord;
	vNormalCoord = aNormalCoord;
	gl_Position = vPos;
}

