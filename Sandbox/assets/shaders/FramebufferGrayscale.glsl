// Framebuffer Shader with Grayscale post-processing

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
	vec3 col = texture(u_ScreenTexture, v_TexCoords).rgb;
	// Combine the three colors to get gray intensity scale. Colors are weighed because human eye is more sensitive to green then blue
	float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	color = vec4(average, average, average, 1.0);
}