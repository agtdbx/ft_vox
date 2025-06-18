#version 450

// Input from vertex
layout(location = 0) in vec2 fragTexCoord;

// Output
layout(location = 0) out vec4   outColor;

// Binding buffer
layout(binding = 1) uniform sampler2D sampleSky;

// Main
void main()
{
    outColor = texture(sampleSky, fragTexCoord);
}
