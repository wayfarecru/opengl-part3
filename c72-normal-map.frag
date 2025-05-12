#version 330

in vec4 vPos;
in vec4 vNorm;
in vec3 vTexCoord; // texture coordinate cubemap : varing
in vec3 vNormalCoord; // normal vector map coordinate
in vec3 vTangent; // tangent vector
uniform samplerCube texSampler; // texture sampler cubemap
uniform samplerCube normalSampler; // normal map sampler cubemap
out vec4 FragColor;

struct Light {
	vec4 pos;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 att;
};
uniform Light light;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shineness;
};
uniform Material material;

void main(void) { 
	// normal vector calculation
	vec3 N = normalize(vNorm.xyz); // original normal vector
	vec3 T = normalize(vTangent);
	vec3 B = -normalize(cross(N, T)); // RHS -> LHS update
	vec4 n = texture( normalSampler, vNormalCoord );
	n = 2.0 * n - 1.0;
	N = normalize( mat3(T, B, N) * vec3( n.xyz ) );
	// shading calculation
	vec3 L = normalize(light.pos.xyz - vPos.xyz);
	vec3 V = vec3(0.0F, 0.0F, 1.0F);
	vec3 R = reflect(-L, N);
	vec4 ambient = light.ambient * material.ambient;
	float d = length(light.pos.xyz - vPos.xyz);
	float denom = light.att.x + light.att.y * d + light.att.z * d * d;
	vec4 diffuse = max(dot(L, N), 0.0) * light.diffuse * material.diffuse / denom;
	vec4 specular = pow(max(dot(R, V), 0.0), material.shineness) * light.specular * material.specular / denom;
	vec4 color = ambient + diffuse + specular;
	// too dim... multiply for some enhancement
	// old code: FragColor = color * texture( texSampler, vTexCoord );
	FragColor = min(1.5 * color * texture( texSampler, vTexCoord ), vec4(1.0, 1.0, 1.0, 1.0));
}
