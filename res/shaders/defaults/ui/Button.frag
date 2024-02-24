#version 450

//----------------------inputs----------------------
layout (location = 0) in vec4 fragmentPos;
layout (location = 1) in vec2 fragmentUV;
layout (location = 2) flat in vec2 size;
//----------------------outputs----------------------
layout (location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout (binding = 1) uniform MaterialUniform {
    vec4 background_color;
    vec4 border_color;
    float borderWidth;
} material;


void main() {
    bool isBorder = (fragmentUV.x * size.x) < material.borderWidth ||
    (fragmentUV.x * size.x) > size.x - material.borderWidth ||
    (fragmentUV.y * size.y) < material.borderWidth ||
    (fragmentUV.y * size.y) > size.y - material.borderWidth;
    if (!isBorder) {
        outColor = material.background_color;
    }
    else
    {
        outColor = material.border_color;
    }
}