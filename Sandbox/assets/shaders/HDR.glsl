// HDR Lighting Shader

#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = a_TexCoords;
    gl_Position = vec4(a_Position, 1.0);
}

#type pixel
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_hdrBuffer;
uniform bool u_HDR;
uniform float u_Exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_hdrBuffer, TexCoords).rgb;
    if(u_HDR)
    {
        // reinhard
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}