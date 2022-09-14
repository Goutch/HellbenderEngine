#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texture0;
layout(location = 1) in vec2 uv;
void main() {

    outColor = vec4(texture(texture0, uv).rgb,1);
}
