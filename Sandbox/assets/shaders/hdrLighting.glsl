#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

uniform bool u_InverseNormals;

void main()
{
    vs_out.FragPos = vec3(u_Transform * vec4(a_Position, 1.0));   
    vs_out.TexCoords = a_TexCoords;
    
    vec3 n = u_InverseNormals ? -a_Normal : a_Normal;
    
    mat3 normalMatrix = transpose(inverse(mat3(u_Transform)));
    vs_out.Normal = normalize(normalMatrix * n);
    
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type pixel
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[16];
uniform sampler2D u_DiffuseTexture;
uniform vec3 u_ViewPosition;

void main()
{           
    vec3 color = texture(u_DiffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
    FragColor = vec4(ambient + lighting, 1.0);
}