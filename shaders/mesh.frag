#version 450

// Images input
layout(binding = 1) uniform UniformBufferObject {
    ivec4 cubes[8192]; // 8192 * 4 = 32768 ints
} ubo;
layout(binding = 2) uniform sampler2D sampleGrass;
layout(binding = 3) uniform sampler2D sampleDirt;
layout(binding = 4) uniform sampler2D sampleStone;
layout(binding = 5) uniform sampler2D sampleWater;
layout(binding = 6) uniform sampler2D sampleSnow;
layout(binding = 7) uniform sampler2D sampleIce;
layout(binding = 8) uniform sampler2D sampleSand;
layout(binding = 9) uniform sampler2D sampleLava;
layout(binding = 10) uniform sampler2D sampleIron;
layout(binding = 11) uniform sampler2D sampleDiamond;

// Input from vertex
layout(location = 0) in vec3    fragPosition;
layout(location = 1) in vec3    fragNormal;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function
vec4    getCubeTexture(vec2 texCoord, int cubeType)
{
    if (cubeType == 1)
        return (texture(sampleGrass, texCoord));
    else if (cubeType == 2)
        return (texture(sampleDirt, texCoord));
    else if (cubeType == 3)
        return (texture(sampleStone, texCoord));
    else if (cubeType == 4)
        return (texture(sampleWater, texCoord));
    else if (cubeType == 5)
        return (texture(sampleSnow, texCoord));
    else if (cubeType == 6)
        return (texture(sampleIce, texCoord));
    else if (cubeType == 7)
        return (texture(sampleSand, texCoord));
    else if (cubeType == 8)
        return (texture(sampleLava, texCoord));
    else if (cubeType == 9)
        return (texture(sampleIron, texCoord));
    else if (cubeType == 10)
        return (texture(sampleDiamond, texCoord));
    else
        return (vec4(1, 0, 1, 1));
}


vec4    getUpColor()
{
    vec2    texCoord = fragPosition.xz;

    int x = int(fragPosition.x);
    int y = int(fragPosition.y - 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getDownColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.z);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y + 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getLeftColor()
{
    vec2    texCoord =  vec2(fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x + 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getRightColor()
{
    vec2    texCoord = vec2(-fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x - 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getFrontColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z - 0.01);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getBackColor()
{
    vec2    texCoord = vec2(-fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z + 0.01);
    int cubeId = x + y * 32 + z * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTexture(texCoord, cubeType));
}


vec4    getColor()
{
    vec4    color = vec4(1, 1, 1, 1);

    if (fragNormal.y > 0.99)
        color = getUpColor();
    else if (fragNormal.y < -0.99)
        color = getDownColor() * 0.4;
    else if (fragNormal.x > 0.99)
        color = getRightColor() * 0.7;
    else if (fragNormal.x < -0.99)
        color = getLeftColor() * 0.7;
    else if (fragNormal.z > 0.99)
        color = getFrontColor() * 0.8;
    else if (fragNormal.z < -0.99)
        color = getBackColor() * 0.6;

    return (color);
}

// Main
void main()
{
    outColor = getColor();
}
