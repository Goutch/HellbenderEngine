#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D[4] layers;

layout(binding = 1) uniform UniformBufferObject {
    int layer_count;
} ubo;
layout(location = 1) in vec2 uv;
void main() {
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < ubo.layer_count; i++) {
        c += (1-c.a)*texture(layers[i], uv);
    }
    outColor = c;
}
