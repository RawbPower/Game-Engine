// Framebuffer Shader with Inversion post-processing

#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main() {
	v_TexCoords = a_TexCoords;
	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}

#type pixel
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

void main() {
	vec3 col = 1.0 - texture(u_ScreenTexture, v_TexCoords).rgb;
	color = vec4(col, 1.0);
}