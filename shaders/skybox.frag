#version 450

// Input from vertex
layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoord;

// Output
layout(location = 0) out vec4   outColor;
layout(binding = 1)  uniform sampler2D sampleSky;

// Function

// Main
void main()
{
    outColor = vec4(fragTexCoord, 1, 1);
}
