// Normal Mapping Shader
// We convent everything into tangent space in the vertex shader
// this is quicker than converting just the normals to world space
// because that would be done in the fragment shader every frame

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

uniform vec3 u_LightPosition;
uniform vec3 u_ViewPosition;

out VS_OUT {
	vec3 v_FragmentPosition;
	vec2 v_TexCoords;
	vec3 v_TangentLightPosition;
	vec3 v_TangentViewPosition;
	vec3 v_TangentFragmentPosition;
} vs_out;


void main() {
	// convert everything to world space
	vs_out.v_FragmentPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	vs_out.v_TexCoords = a_TexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(u_Transform)));
	vec3 T = normalize(normalMatrix * a_Tangent);
	vec3 N = normalize(normalMatrix * a_Normal);
	T = normalize(T - dot(T,N)*N);			// make sure tangent is orthogonal - gram schmidt
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T,B,N)); 		// T,B,N are orthogonal => TBN orthogonal => transpose = inverse (transpose is easier than inverse)
	vs_out.v_TangentLightPosition = TBN * u_LightPosition;
	vs_out.v_TangentViewPosition = TBN * u_ViewPosition;
	vs_out.v_TangentFragmentPosition = TBN * vs_out.v_FragmentPosition;

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

out vec4 color;

in VS_OUT {
	vec3 v_FragmentPosition;
	vec2 v_TexCoords;
	vec3 v_TangentLightPosition;
	vec3 v_TangentViewPosition;
	vec3 v_TangentFragmentPosition;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec3 u_LightPosition;
uniform vec3 u_ViewPosition;

void main() {
	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(texture_normal1, fs_in.v_TexCoords).rgb;
	// transform normal vector range to [-1, 1]
	normal = normalize(normal*2.0 - 1.0);		// this normal is in tangent space

	// get diff color
	vec3 col = texture(texture_diffuse1, fs_in.v_TexCoords).rgb;
	//ambient
	vec3 ambient = 0.1 * col;
	// diffuse
	vec3 lightDirection= normalize(fs_in.v_TangentLightPosition - fs_in.v_TangentFragmentPosition);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse= diff * col;
	// specular
	vec3 viewDirection = normalize(fs_in.v_TangentViewPosition - fs_in.v_TangentFragmentPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float spec = pow(max(dot(normal, halfwayDirection), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;
	color = vec4(ambient + diffuse + specular, 1.0);
}