#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
   //outColor = texture(texSampler, fragTexCoord);
    outColor=vec4(fragTexCoord,0.0,1.0);
}