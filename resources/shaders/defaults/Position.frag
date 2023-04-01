#version 450

//----------------------inputs----------------------
layout (location = 0) in vec4 fragmentPos;

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;


void main() {
    outColor = material.color;
}