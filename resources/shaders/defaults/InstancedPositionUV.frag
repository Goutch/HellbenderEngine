#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;

void main() {
    outColor=material.color;
}