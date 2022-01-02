#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1,rgba32f) uniform readonly image2D image;

void main() {
    ivec2 coord=ivec2(fragTexCoord.x<0.5f?0:1, fragTexCoord.y<0.5f?0:1);
    outColor=imageLoad(image, coord);
}