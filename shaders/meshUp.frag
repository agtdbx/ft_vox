#version 450

// Images input
layout(binding = 1) uniform UniformBufferObject {
    ivec4 cubes[1024]; // 1024 * 4 = 4096 ints
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

// Output
layout(location = 0) out vec4   outColor;

// Constants
const vec3  normal = vec3(0, 1, 0);

// Function
vec4    getColor(vec2 texCoord, int cubeType)
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

// Main
void main() {
    vec2    texCoord = fragPosition.xz;

    int x = int(fragPosition.x);
    int y = int(fragPosition.y + 0.01);
    int z = int(fragPosition.z);
    int cubeId = x + y * 16 + z * 256;
    int cubeType = ubo.cubes[cubeId >> 2][cubeId % 4]; // >> 2 = / 4

    outColor = getColor(texCoord, cubeType);
}
