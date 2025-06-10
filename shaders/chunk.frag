#version 450

// Images input
layout(binding = 1) uniform UniformBufferObject {
    ivec4 cubes[65536]; // 65536 * 4 = 262144
} ubo;
layout(binding = 2)  uniform sampler2D sampleGrassUp;
layout(binding = 3)  uniform sampler2D sampleDirtUp;
layout(binding = 4)  uniform sampler2D sampleStoneUp;
layout(binding = 5)  uniform sampler2D sampleWaterUp;
layout(binding = 6)  uniform sampler2D sampleSnowUp;
layout(binding = 7)  uniform sampler2D sampleIceUp;
layout(binding = 8)  uniform sampler2D sampleSandUp;
layout(binding = 9)  uniform sampler2D sampleLavaUp;
layout(binding = 10) uniform sampler2D sampleIronUp;
layout(binding = 11) uniform sampler2D sampleDiamondUp;
layout(binding = 12) uniform sampler2D sampleGrassSide;
layout(binding = 13) uniform sampler2D sampleDirtSide;
layout(binding = 14) uniform sampler2D sampleStoneSide;
layout(binding = 15) uniform sampler2D sampleWaterSide;
layout(binding = 16) uniform sampler2D sampleSnowSide;
layout(binding = 17) uniform sampler2D sampleIceSide;
layout(binding = 18) uniform sampler2D sampleSandSide;
layout(binding = 19) uniform sampler2D sampleLavaSide;
layout(binding = 20) uniform sampler2D sampleIronSide;
layout(binding = 21) uniform sampler2D sampleDiamondSide;
layout(binding = 22) uniform sampler2D sampleGrassDown;
layout(binding = 23) uniform sampler2D sampleDirtDown;
layout(binding = 24) uniform sampler2D sampleStoneDown;
layout(binding = 25) uniform sampler2D sampleWaterDown;
layout(binding = 26) uniform sampler2D sampleSnowDown;
layout(binding = 27) uniform sampler2D sampleIceDown;
layout(binding = 28) uniform sampler2D sampleSandDown;
layout(binding = 29) uniform sampler2D sampleLavaDown;
layout(binding = 30) uniform sampler2D sampleIronDown;
layout(binding = 31) uniform sampler2D sampleDiamondDown;

// Input from vertex
layout(location = 0) in vec3    fragPosition;
layout(location = 1) in vec3    fragNormal;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function
vec4    getCubeTextureUp(vec2 texCoord, int cubeType)
{
    if (cubeType == 1)
        return (texture(sampleGrassUp, texCoord));
    else if (cubeType == 2)
        return (texture(sampleDirtUp, texCoord));
    else if (cubeType == 3)
        return (texture(sampleStoneUp, texCoord));
    else if (cubeType == 4)
        return (texture(sampleWaterUp, texCoord));
    else if (cubeType == 5)
        return (texture(sampleSnowUp, texCoord));
    else if (cubeType == 6)
        return (texture(sampleIceUp, texCoord));
    else if (cubeType == 7)
        return (texture(sampleSandUp, texCoord));
    else if (cubeType == 8)
        return (texture(sampleLavaUp, texCoord));
    else if (cubeType == 9)
        return (texture(sampleIronUp, texCoord));
    else if (cubeType == 10)
        return (texture(sampleDiamondUp, texCoord));
    else
        return (vec4(1, 0, 1, 1));
}


vec4    getCubeTextureSide(vec2 texCoord, int cubeType)
{
    if (cubeType == 1)
        return (texture(sampleGrassSide, texCoord));
    else if (cubeType == 2)
        return (texture(sampleDirtSide, texCoord));
    else if (cubeType == 3)
        return (texture(sampleStoneSide, texCoord));
    else if (cubeType == 4)
        return (texture(sampleWaterSide, texCoord));
    else if (cubeType == 5)
        return (texture(sampleSnowSide, texCoord));
    else if (cubeType == 6)
        return (texture(sampleIceSide, texCoord));
    else if (cubeType == 7)
        return (texture(sampleSandSide, texCoord));
    else if (cubeType == 8)
        return (texture(sampleLavaSide, texCoord));
    else if (cubeType == 9)
        return (texture(sampleIronSide, texCoord));
    else if (cubeType == 10)
        return (texture(sampleDiamondSide, texCoord));
    else
        return (vec4(1, 0, 1, 1));
}


vec4    getCubeTextureDown(vec2 texCoord, int cubeType)
{
    if (cubeType == 1)
        return (texture(sampleGrassDown, texCoord));
    else if (cubeType == 2)
        return (texture(sampleDirtDown, texCoord));
    else if (cubeType == 3)
        return (texture(sampleStoneDown, texCoord));
    else if (cubeType == 4)
        return (texture(sampleWaterDown, texCoord));
    else if (cubeType == 5)
        return (texture(sampleSnowDown, texCoord));
    else if (cubeType == 6)
        return (texture(sampleIceDown, texCoord));
    else if (cubeType == 7)
        return (texture(sampleSandDown, texCoord));
    else if (cubeType == 8)
        return (texture(sampleLavaDown, texCoord));
    else if (cubeType == 9)
        return (texture(sampleIronDown, texCoord));
    else if (cubeType == 10)
        return (texture(sampleDiamondDown, texCoord));
    else
        return (vec4(1, 0, 1, 1));
}


vec4    getUpColor()
{
    vec2    texCoord = fragPosition.xz;

    int x = int(fragPosition.x);
    int y = int(fragPosition.y - 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureUp(texCoord, cubeType));
}


vec4    getDownColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.z);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y + 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureDown(texCoord, cubeType));
}


vec4    getLeftColor()
{
    vec2    texCoord =  vec2(fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x + 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getRightColor()
{
    vec2    texCoord = vec2(-fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x - 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getFrontColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z - 0.01);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getBackColor()
{
    vec2    texCoord = vec2(-fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z + 0.01);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    return (getCubeTextureSide(texCoord, cubeType));
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
