#version 450

//----------------------inputs----------------------
layout (location = 0) in vec4 fragmentPos;
layout(location = 1) in vec2 fragmentUV;

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;


void main() {
    outColor = material.color;
}