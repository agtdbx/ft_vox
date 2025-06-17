#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    pos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uint inCubeType;

layout(location = 0) out vec3       fragPosition;
layout(location = 1) out vec3       fragNormal;
layout(location = 2) out flat uint  fragCubeType;

void main() {
    gl_Position = ubo.proj * ubo.view * (ubo.model * vec4(inPosition, 1.0) + ubo.pos);
    fragPosition = inPosition;
    fragNormal = inNormal;
    fragCubeType = inCubeType;
}
