// Reflecting Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec3 v_Position;

void main() {
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	v_Position = vec3(u_Transform * vec4(a_Position, 1.0));
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

out vec4 color;

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 u_ViewPosition;
uniform samplerCube u_Skybox;

void main() {
	vec3 I = normalize(v_Position - u_ViewPosition);	// Incident ray
	vec3 R = reflect(I, normalize(v_Normal));			// Reflected ray
	color = vec4(texture(u_Skybox, R).rgb, 1.0);
}