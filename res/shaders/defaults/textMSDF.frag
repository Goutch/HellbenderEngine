#version 450
//https://github.com/Chlumsky/msdfgen
layout(location = 1) in vec2 fragmentUV;

layout(binding = 1)uniform sampler2D msdf;


layout(location = 0) out vec4 outColor;
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float range=1.2;
void main() {
    vec3 msd = texture(msdf, fragmentUV).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = range*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    outColor = vec4(1,1,1,opacity);

}