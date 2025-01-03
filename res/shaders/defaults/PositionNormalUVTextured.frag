#version 450
#extension GL_EXT_nonuniform_qualifier: enable
//----------------------inputs----------------------
layout (location = 0) in vec4 fragmentPos;
layout (location = 1) in vec4 fragmentNormal;
layout (location = 2) in vec2 fragmentUV;

//----------------------outputs----------------------
layout (location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout (binding = 1) uniform usampler2D albedo;

layout (binding = 2) uniform MaterialProperties {
    vec4 base_color;
    vec4 emmisive_factor;
    float metal_factor;
    float roughnes;
    uint has_albedo;
    uint has_metallic_roughness;
    uint has_emmisive;
    uint has_normal;
    uint has_occlusion;
} material;

void main() {
    if (material.has_albedo == 1)
    {
        outColor = texture(albedo, fragmentUV) / 255.0f;
    }
    else
    {
        outColor = material.base_color;
    }
}