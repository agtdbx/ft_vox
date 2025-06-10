#version 450

// Images input

// Input from vertex
layout(location = 0) in vec3    fragPosition;
layout(location = 1) in vec3    fragNormal;

// Output
layout(location = 0) out vec4   outColor;

// Constants

// Function

// Main
void main()
{
    outColor = vec4((fragNormal + vec3(1, 1, 1)) / 2, 1);
}
