// Skybox Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_TexCoords;

void main() {
	v_TexCoords = a_Position;
	vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0);
	gl_Position = pos.xyww;			// make z component = w component and there for after perspective projection it is 1
}

#type pixel
#version 330 core

out vec4 color;

in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

void main() {
	color = texture(u_Skybox, v_TexCoords);
}