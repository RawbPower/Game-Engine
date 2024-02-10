// PBR Shader

#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;
layout (location = 2) in vec3 a_Normal;

out vec2 v_TexCoords;
out vec3 v_WorldPosition;
out vec3 v_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    v_TexCoords = a_TexCoords;
    v_WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0));
    v_Normal = mat3(u_Transform) * a_Normal;

    gl_Position = u_ViewProjection * vec4(v_WorldPosition, 1.0);
}

#type pixel
#version 330 core
out vec4 FragmentColor;

in vec2 v_TexCoords;
in vec3 v_WorldPosition;
in vec3 v_Normal;

// material parameters
uniform vec3 u_Albedo;
uniform vec3 u_AlbedoB;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_Ao;

// light
uniform vec3 u_LightPositions[1];
uniform vec3 u_LightColors[1];

uniform vec3 u_ViewPosition;

uniform sampler2D u_Texture;

const float PI = 3.14159265359;

// Use normal distribution to calculate the alignment of microfacets
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}


// Combination of the Schlick and GGX function to calculate the about of light that gets obscured from your vision
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// Account for the geometry of both the view and light direction vectors
float GeomtrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Calculates the reflection coefficient from the Fresnel equation
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    if(cosTheta>1.0)
        cosTheta=1.0;

    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() 
{   
    vec3 N = normalize(v_Normal);                               // Normal to interface
    vec3 V = normalize(u_ViewPosition - v_WorldPosition);       // View direction

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, u_Albedo, u_Metallic);         // mixes the two parameters with the metallic ratio

    // reflection equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(u_LightPositions[i] - v_WorldPosition);         // Light direction
        vec3 H = normalize(V + L);                                      // Halfway direction
        float distance = length(u_LightPositions[i] - v_WorldPosition);
        float attenuation = 1.0 / (distance * distance);                // Usual inverse square law
        vec3 radiance = u_LightColors[i] * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, u_Roughness);                 // Normal distribution coefficient
        float G = GeomtrySmith(N, V, L, u_Roughness);                   // Geometry coefficient
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);               // Fresnel Equation coefficient (reflection ratio)

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - u_Metallic;                                           // Metals only reflect so kD = 0 for "full metal"

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * u_Albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = u_Albedo * u_Ao;

    vec4 textureColor = texture(u_Texture, v_TexCoords);
    if (textureColor.r > 0.5f)
    {
        ambient = vec3(0.5) * u_Albedo * u_Ao;
    }
    else
    {
        ambient = vec3(0.5) * u_AlbedoB * u_Ao;
    }

    vec3 color = (ambient + Lo);

    color = color / (color + vec3(1.0));                        // Reinhard tone mapping
    color = pow(color, vec3(1.0/2.2));                          // Gamme correction

    FragmentColor = vec4(color, 1.0);
}