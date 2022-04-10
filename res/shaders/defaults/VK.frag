#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1, set = 0) uniform sampler2D texture0;

layout(binding =2, set =0) uniform MaterialBuffer {
    vec4 color;
} material;

void main() {
    outColor=vec4(material.color);//texture(texture0, fragTexCoord);
}