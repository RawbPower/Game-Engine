// Simple Model Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main() {
	v_TexCoords = a_TexCoords;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

out vec4 color;

in vec2 v_TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
	color = texture(texture_diffuse1, v_TexCoords);
}