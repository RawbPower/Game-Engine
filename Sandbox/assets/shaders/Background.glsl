// PBR and HDR Background Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_WorldPos;

void main() {
	v_WorldPos = a_Position;
	
	mat4 rotView = mat4(mat3(u_View));
	vec4 clipPos = u_Projection * rotView * vec4(v_WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}

#type pixel
#version 330 core

out vec4 FragColor;

in vec3 v_WorldPos;

uniform samplerCube u_EnvironmentMap;

void main() {
	vec3 envColor = textureLod(u_EnvironmentMap, v_WorldPos, 0.0).rgb;

	// HDR tonemap and gamma correct
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0/2.2));

	FragColor = vec4(envColor, 1.0);
}