// Point Lighting Shader for 3D Model

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

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 2

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float texture_shininess;
uniform Light light[NR_POINT_LIGHTS];

uniform vec3 u_ViewPosition;

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec2 v_TexCoords;

void main() {

	vec3 result = vec3(0.0f);
	vec3 norm = normalize(v_Normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_FragmentPosition);

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		vec3 lightDir = normalize(light[i].position - v_FragmentPosition);
    	// diffuse shading
    	float diff = max(dot(norm, lightDir), 0.0);
    	// specular shading
    	vec3 reflectDir = reflect(-lightDir, norm);
    	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), texture_shininess);
    	// attenuation
    	float distance    = length(light[i].position - v_FragmentPosition);
    	float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + 
  			     light[i].quadratic * (distance * distance));    
    	// combine results
    	vec3 ambient  = light[i].ambient  * texture(texture_diffuse1, v_TexCoords).rgb;
    	vec3 diffuse  = light[i].diffuse  * diff * texture(texture_diffuse1, v_TexCoords).rgb;
    	vec3 specular = light[i].specular * spec * texture(texture_specular1, v_TexCoords).rbg;
    	ambient  *= attenuation;
   	 	diffuse  *= attenuation;
    	specular *= attenuation;
    	result += (ambient + diffuse + specular);
	}

	color = vec4(result, 1.0);
}