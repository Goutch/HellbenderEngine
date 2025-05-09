#version 450
//--------------------inputs--------------------
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;



//--------------------outputs--------------------
layout(location = 0) out vec3 fragmentPos;
layout(location = 1) out vec2 fragmentUV;
layout(location = 2) out vec3 fragmentNormal;

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
    vec4 pos = constants.transform * vec4(inPosition.xyz, 1.0);
    gl_Position = ubo.projection * ubo.view * pos;
    fragmentPos = pos.xyz;
    fragmentNormal = inNormal;
    fragmentUV = inUV;
}