#version 450

layout(binding = 1) uniform UniformBufferObject
{
    vec4    textColor;
    vec4    backgroundColor;
} ubo;

// Input from vertex
layout(location = 0) in vec2 fragTexCoord;

// Output
layout(location = 0) out vec4   outColor;

// Binding buffer
layout(binding = 2) uniform sampler2D sampleFont;

// Main
void main()
{
    vec4 color = texture(sampleFont, fragTexCoord);

    if (color == vec4(0, 0, 0, 0))
        color = ubo.backgroundColor;
    else
        color *= ubo.textColor;
    outColor = color;
}
