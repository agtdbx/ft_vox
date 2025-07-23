#version 450

// Buffer input
layout(binding = 1) uniform UniformBufferObject {
    float   frameId;
} ubo;

// Images input
layout(binding = 2) uniform sampler2DArray sampleTexturesWater;
layout(binding = 3) uniform sampler2DArray sampleTexturesLava;

// Input from vertex
layout(location = 0) in vec3        fragPosition;
layout(location = 1) in vec3        fragNormal;
layout(location = 2) in flat uint   fragCubeType;

// Output
layout(location = 0) out vec4   outColor;

// Functions
vec4    getPixelTexture(vec2 texCoord)
{
    if (fragCubeType == 16) // Water
        return (texture(sampleTexturesWater, vec3(texCoord, int(ubo.frameId))));
    else if (fragCubeType == 17) // Lava
    {
        vec4    color = texture(sampleTexturesLava, vec3(texCoord, int(ubo.frameId)));
        color.a = 0.9;
        return (color);
    }
    else
        return (vec4(0.5, 0.5, 0.5, 0.5));
}

vec4    getColor()
{
    vec4    color = vec4(1, 1, 1, 1);
    vec2    texCoord;

    if (fragNormal.y > 0.99) //  Up
    {
        texCoord = fragPosition.xz;
        color = getPixelTexture(texCoord);
    }
    else if (fragNormal.y < -0.99) // Down
    {
        texCoord = vec2(fragPosition.x, -fragPosition.z);
        color = getPixelTexture(texCoord);
    }
    else if (fragNormal.x > 0.99) // Right
    {
        texCoord = vec2(-fragPosition.z, -fragPosition.y);
        color = getPixelTexture(texCoord);
    }
    else if (fragNormal.x < -0.99) // Left
    {
        texCoord =  vec2(fragPosition.z, -fragPosition.y);
        color = getPixelTexture(texCoord);
    }
    else if (fragNormal.z > 0.99) // Front
    {
        texCoord = vec2(fragPosition.x, -fragPosition.y);
        color = getPixelTexture(texCoord);
    }
    else if (fragNormal.z < -0.99) // Back
    {
        texCoord = vec2(-fragPosition.x, -fragPosition.y);
        color = getPixelTexture(texCoord);
    }

    return (color);
}

// Main
void main()
{
    outColor = getColor();
}
