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


struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

#define NR_POINT_LIGHTS 4

uniform Material material;

uniform vec3 u_ViewPosition;
uniform PointLight lights[NR_POINT_LIGHTS];

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec2 v_TexCoords;

// function definitions
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

void main() {

	// properties
	vec3 norm = normalize(v_Normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_FragmentPosition);

    vec3 result;

	// Point Lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(lights[i], norm, v_FragmentPosition, viewDirection);
	}

	color = vec4(result, 1.0);

}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * texture(material.diffuse, v_TexCoords).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse, v_TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, v_TexCoords).rbg;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 
