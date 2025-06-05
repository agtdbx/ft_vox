#version 450

// Images input
layout(binding = 1) uniform sampler2D texSampler;

// Input from vertex
layout(location = 0) in vec3 fragPosition;

// Output
layout(location = 0) out vec4 outColor;

// Constants
vec3    normal = vec3(0, 1, 0);

void main() {
    vec2    texCoord = fragPosition.xz;
    outColor = texture(texSampler, texCoord);
}
