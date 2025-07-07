#version 450

// Images input
layout(binding = 1) uniform sampler2DArray sampleCubeTextures;

// Input from vertex
layout(location = 0) in vec3        fragPosition;
layout(location = 1) in vec3        fragNormal;
layout(location = 2) in flat uint   fragCubeType;

// Output
layout(location = 0) out vec4   outColor;

// Functions
vec4    getColor()
{
    vec4    color = vec4(1, 1, 1, 1);
    vec2    texCoord;

    if (fragNormal.y > 0.99) //  Up
    {
        texCoord = fragPosition.xz;
        color = texture(sampleCubeTextures, vec3(texCoord, fragCubeType));
    }
    else if (fragNormal.y < -0.99) // Down
    {
        texCoord = vec2(fragPosition.x, -fragPosition.z);
        color = texture(sampleCubeTextures, vec3(texCoord, 20 + fragCubeType)) * 0.6;
    }
    else if (fragNormal.x > 0.99) // Right
    {
        texCoord = vec2(-fragPosition.z, -fragPosition.y);
        color = texture(sampleCubeTextures, vec3(texCoord, 10 + fragCubeType)) * 0.85;
    }
    else if (fragNormal.x < -0.99) // Left
    {
        texCoord =  vec2(fragPosition.z, -fragPosition.y);
        color = texture(sampleCubeTextures, vec3(texCoord, 10 + fragCubeType)) * 0.85;
    }
    else if (fragNormal.z > 0.99) // Front
    {
        texCoord = vec2(fragPosition.x, -fragPosition.y);
        color = texture(sampleCubeTextures, vec3(texCoord, 10 + fragCubeType)) * 0.9;
    }
    else if (fragNormal.z < -0.99) // Back
    {
        texCoord = vec2(-fragPosition.x, -fragPosition.y);
        color = texture(sampleCubeTextures, vec3(texCoord, 10 + fragCubeType)) * 0.8;
    }

    return (color);
}

// Main
void main()
{
    outColor = getColor();
}
