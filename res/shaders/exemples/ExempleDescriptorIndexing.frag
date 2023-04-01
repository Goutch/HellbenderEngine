#version 450
#extension GL_EXT_nonuniform_qualifier: enable

//----------------------inputs----------------------
layout (location = 0) in vec4 fragmentPos;

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout(binding = 1, set = 0) uniform UniformBufferObject {
    int i;
} materialIndex;

layout(binding = 2, set = 0) readonly sampler2D textures[];


void main() {
    outColor = texture(textures[materialIndex.i]);
}