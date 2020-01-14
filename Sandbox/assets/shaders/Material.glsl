// Basic Specular Lighting Shader

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
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout(location = 0) out vec4 color;

uniform Material material;
uniform Light light;

uniform vec3 u_ViewPosition;

in vec3 v_Normal;
in vec3 v_FragmentPosition;

void main() {
	// ambient 
	vec3 ambient = light.ambient * material.ambient;	

	// diffusive
	vec3 norm = normalize(v_Normal);
	vec3 lightDirection = normalize(light.position - v_FragmentPosition);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDirection = normalize(u_ViewPosition - v_FragmentPosition);
	vec3 reflectDirection = reflect(-lightDirection, norm);						// sign infront of lightDir depends on how it was set. We want vector from light source to fragment
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * (material.specular * spec);

	// final color
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0);
}