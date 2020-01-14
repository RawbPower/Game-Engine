// Basic Diffusive Lighting Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec3 v_FragmentPosition;

void main() {
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_FragmentPosition = vec3(u_Transform * vec4(a_Position, 1.0));
}

#type pixel
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;

in vec3 v_Normal;
in vec3 v_FragmentPosition;

void main() {
	// ambient 
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_LightColor;	

	// diffusive
	vec3 norm = normalize(v_Normal);
	vec3 lightDirection = normalize(u_LightPosition - v_FragmentPosition);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = diff * u_LightColor;

	// final color
	vec3 result = (ambient + diffuse) * u_ObjectColor;
	color = vec4(result, 1.0);
}