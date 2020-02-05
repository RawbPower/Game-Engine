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

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main() {
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(v_WorldPos);

    vec3 irradiance = vec3(0.0);

	// tangent space calculation from origin point
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}