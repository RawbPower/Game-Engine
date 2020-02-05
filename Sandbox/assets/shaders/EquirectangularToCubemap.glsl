// Converts hdr equirectangular map to cube map

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_WorldPos;

void main() {
	v_WorldPos = a_Position;
	gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core

out vec4 FragColor;

in vec3 v_WorldPos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);		// vec2(1/2π, 1/π)

// Converts the pitch from [-π, π] to [-0.5, 0.5] and yaw from [-π/2, π/2] to [-0.5, 0.5]
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main() {
	vec2 uv = SampleSphericalMap(normalize(v_WorldPos));
	vec3 color = texture(u_EquirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}