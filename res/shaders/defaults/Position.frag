#version 450

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;


void main() {
    outColor = material.color;
}