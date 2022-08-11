#version 450

//--------------------inputs--------------------
layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec2 inTexCoord;


//--------------------outputs--------------------
//layout(location = 1) out vec2 fragTexCoord;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 normal;
//----------------------uniforms----------------------
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

layout(push_constant) uniform Constants
{
    mat4 transform;
} constants;

void main() {
    normal = inNormal;
    vec4 pos = constants.transform * vec4(inPosition, 1.0);
    fragPos= pos;
    gl_Position = ubo.projection * ubo.view * pos;
}