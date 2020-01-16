// Basic Lighting Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main() {
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_LightColor;

void main() {
	color = vec4(u_LightColor, 1.0);
}