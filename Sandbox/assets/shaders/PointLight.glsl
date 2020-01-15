// Basic Specular Lighting Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec3 v_FragmentPosition;
out vec2 v_TexCoords;

void main() {
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_FragmentPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	v_TexCoords = a_TexCoords;
}

#type pixel
#version 330 core
out vec4 color;	

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

uniform vec3 u_ViewPosition;

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec2 v_TexCoords;

void main() {
	// ambient 
	vec3 ambient = light.ambient * texture(material.diffuse, v_TexCoords).rgb;	

	// diffusive
	vec3 norm = normalize(v_Normal);
	vec3 lightDirection = normalize(light.position - v_FragmentPosition);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, v_TexCoords).rgb;

	// specular
	vec3 viewDirection = normalize(u_ViewPosition - v_FragmentPosition);
	vec3 reflectDirection = reflect(-lightDirection, norm);						// sign infront of lightDir depends on how it was set. We want vector from light source to fragment
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, v_TexCoords).rgb;

	// attenuation
	float distance = length(light.position - v_FragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// final color
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0);
}