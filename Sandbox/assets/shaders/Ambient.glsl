// Basic Ambient Lighting Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main() {
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_LightColor;	

	vec3 result = ambient * u_ObjectColor;
	color = vec4((ambientStrength * u_LightColor) * u_ObjectColor, 1.0);
}