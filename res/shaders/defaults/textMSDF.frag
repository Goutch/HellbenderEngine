#version 450
//https://github.com/Chlumsky/msdfgen
layout (location = 1) in vec2 fragmentUV;

layout (binding = 1) uniform sampler2D mtsdf;

layout (binding = 2) uniform Material {
    vec4 color;
} material;

layout (location = 0) out vec4 outColor;
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float range = 2.0;
void main() {
    vec3 msd = texture(mtsdf, fragmentUV).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = range * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    if (opacity < 0.01)discard;
    outColor = vec4(material.color.rgb, opacity * material.color.a);
    //outColor = vec4(fragmentUV, 0.0, 1.0);
}