#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform MaterialUniform {
    vec4 color;
} material;

void main() {
    if (distance(fragTexCoord-vec2(0.5, 0.5), vec2(0, 0))>0.5)
    {
        discard;
    }
    else {
        outColor=material.color;
    }

}