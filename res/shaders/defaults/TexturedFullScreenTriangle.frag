#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D[4] layers;

layout(binding = 1) uniform UniformBufferObject {
    int layer_count;
} ubo;
layout(location = 1) in vec2 uv;
void main() {
    vec4 c = texture(layers[ubo.layer_count], uv);
    for (int i = ubo.layer_count-1; i >= 0; i--) {
        c += texture(layers[i], uv)*(1.0-c.a);
    }
    outColor = c;
    // outColor =  vec4(pow(c.rgb,vec3(2.2)), 1);
}
