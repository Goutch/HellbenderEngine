#version 450
//--------------------inputs--------------------
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;


//--------------------outputs--------------------
layout(location = 0) out vec4 fragmentPos;
layout(location = 2) out vec2 fragmentUV;
layout(location = 3) out vec3 fragmentNormal;

//----------------------uniforms----------------------
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

layout(push_constant) uniform Constants
{
    mat4 transform;
} constants;

layout(binding = 1) uniform MaterialUniform {
    vec4 color;
    bool hasTexture;
} material;


void main() {
    vec4 pos = constants.transform * vec4(inPosition, 1.0);
    gl_Position = ubo.projection * ubo.view * pos;
    fragmentPos = pos;
    fragmentNormal = inNormal;
    fragmentUV = inUV;
}