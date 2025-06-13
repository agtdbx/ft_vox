#version 450

// Cubes array input
layout(std430, binding = 1) readonly buffer cubesBuffer {
    uint cubes[65536]; // 65536 * 4 = 262144
};
// Images input
layout(binding = 2) uniform sampler2DArray sampleCubeTextures;

// Input from vertex
layout(location = 0) in vec3    fragPosition;
layout(location = 1) in vec3    fragNormal;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function
vec4    getCubeTextureUp(vec2 texCoord, uint cubeType)
{
    if  (cubeType < 1 || cubeType  > 10)
        return (vec4(1, 0, 1, 1));

    return (texture(sampleCubeTextures, vec3(texCoord, cubeType - 1)));
}


vec4    getCubeTextureSide(vec2 texCoord, uint cubeType)
{
    if  (cubeType < 1 || cubeType  > 10)
        return (vec4(1, 0, 1, 1));

    return (texture(sampleCubeTextures, vec3(texCoord, 10 + cubeType - 1)));
}


vec4    getCubeTextureDown(vec2 texCoord, uint cubeType)
{
    if  (cubeType < 1 || cubeType  > 10)
        return (vec4(1, 0, 1, 1));

    return (texture(sampleCubeTextures, vec3(texCoord, 20 + cubeType - 1)));
}


uint    getCubeType(int x, int y, int z)
{
    int cubeId = x + z * 32 + y * 1024;
    uint cubeType = (cubes[cubeId >> 2] >> (8 * (cubeId & 3))) & 255;

    return (cubeType);
}


vec4    getUpColor()
{
    vec2    texCoord = fragPosition.xz;

    int x = int(fragPosition.x);
    int y = int(fragPosition.y - 0.01);
    int z = int(fragPosition.z);
    uint cubeType = getCubeType(x, y, z);

    return (getCubeTextureUp(texCoord, cubeType));
}


vec4    getDownColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.z);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y + 0.01);
    int z = int(fragPosition.z);
    uint cubeType = getCubeType(x, y, z);

    return (getCubeTextureDown(texCoord, cubeType));
}


vec4    getLeftColor()
{
    vec2    texCoord =  vec2(fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x + 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    uint cubeType = getCubeType(x, y, z);

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getRightColor()
{
    vec2    texCoord = vec2(-fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x - 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    uint cubeType = getCubeType(x, y, z);

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getFrontColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z - 0.01);
    uint cubeType = getCubeType(x, y, z);

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getBackColor()
{
    vec2    texCoord = vec2(-fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z + 0.01);
    uint cubeType = getCubeType(x, y, z);

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
