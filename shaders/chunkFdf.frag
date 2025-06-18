#version 450

// Images input

// Input from vertex
layout(location = 0) in vec3        fragPosition;
layout(location = 1) in vec3        fragNormal;
layout(location = 2) in flat uint   fragCubeType;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function

// Main
void main()
{
    outColor = vec4((fragNormal + vec3(1, 1, 1)) / 2, 1);
}
