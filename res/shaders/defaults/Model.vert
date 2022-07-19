#version 450

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
//layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normal;
layout(location = 1) out vec3 fragPos;
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

layout(push_constant) uniform Constants
{
    mat4 transform;
} constants;

void main() {
    FragPos =ubo.projection * ubo.view * constants.transform* vec3(model * vec4(aPos, 1.0));
    normal = inNormal;

    gl_Position =  FragPos;
}