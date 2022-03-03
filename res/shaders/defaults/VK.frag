#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texture0;

void main() {
    outColor=vec4(1,0,0,1);//texture(texture0, fragTexCoord);
}