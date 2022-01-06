#version 450
#extension GL_GOOGLE_include_directive : require

#include "../../res/shaders/Intersection.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 3) in vec3 worldPosition;

layout(binding = 1, rgba32f) uniform readonly image2D image;


void main() {
    ivec2 coord=ivec2(fragTexCoord.x<0.5f?0:1, fragTexCoord.y<0.5f?0:1);
    outColor=vec4(worldPosition/25.,1.);
    //outColor=imageLoad(image, coord);
}