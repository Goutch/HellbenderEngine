#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;
layout(push_constant) uniform Constants
{
    mat4 transform;
} constants;

void main() {
    gl_Position = ubo.projection * ubo.view * constants.transform * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
}