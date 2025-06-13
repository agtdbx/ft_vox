#version 450

// Cubes array input
layout(std430, binding = 1) buffer cubesBuffer {
    int cubes[262144];
};
// Images input
layout(binding = 2)  uniform sampler2D sampleCubes;

// Input from vertex
layout(location = 0) in vec3    fragPosition;
layout(location = 1) in vec3    fragNormal;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function
ivec2    transformTexCoord(vec2 texCoord, int cubeType, int face)
{
    texCoord *= 32.0;

    int x = int(texCoord.x) % 32;
    int y = int(texCoord.y) % 32;

    if (x < 0)
        x = 32 + x;
    if (y < 0)
        y = 32 + y;

    ivec2 newCoord = ivec2(x, y);
    return (newCoord);
}

vec4    getCubeTextureUp(vec2 texCoord, int cubeType)
{
    if (cubeType <= 0 || cubeType > 10)
        return (vec4(1, 0, 1, 1));

    ivec2 newCoord = transformTexCoord(texCoord, cubeType, 0);
    return (texture(sampleCubes, newCoord));
}


vec4    getCubeTextureSide(vec2 texCoord, int cubeType)
{
    if (cubeType <= 0 || cubeType > 10)
        return (vec4(1, 0, 1, 1));

    ivec2 newCoord = transformTexCoord(texCoord, cubeType, 1);
    return (texture(sampleCubes, newCoord));
}


vec4    getCubeTextureDown(vec2 texCoord, int cubeType)
{
    if (cubeType <= 0 || cubeType > 10)
        return (vec4(1, 0, 1, 1));

    ivec2 newCoord = transformTexCoord(texCoord, cubeType, 2);
    return (texture(sampleCubes, newCoord));
}


vec4    getUpColor()
{
    vec2    texCoord = fragPosition.xz;

    int x = int(fragPosition.x);
    int y = int(fragPosition.y - 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

    return (getCubeTextureUp(texCoord, cubeType));
}


vec4    getDownColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.z);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y + 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

    return (getCubeTextureDown(texCoord, cubeType));
}


vec4    getLeftColor()
{
    vec2    texCoord =  vec2(fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x + 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getRightColor()
{
    vec2    texCoord = vec2(-fragPosition.z, -fragPosition.y);

    int x = int(fragPosition.x - 0.01);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getFrontColor()
{
    vec2    texCoord = vec2(fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z - 0.01);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

    return (getCubeTextureSide(texCoord, cubeType));
}


vec4    getBackColor()
{
    vec2    texCoord = vec2(-fragPosition.x, -fragPosition.y);

    int x = int(fragPosition.x);
    int y = int(fragPosition.y);
    int z = int(fragPosition.z + 0.01);
    int cubeId = x + z * 32 + y * 1024;
    int cubeType = cubes[cubeId];

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
