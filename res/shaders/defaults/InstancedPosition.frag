#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;

void main() {
    outColor=material.color;
}