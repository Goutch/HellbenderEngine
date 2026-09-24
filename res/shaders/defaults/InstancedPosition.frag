#version 450

layout(location = 0) out vec4 outColor;
layout (location = 0) in flat int fragInstanceID;
void main() {
    outColor = vec4(mod(fragInstanceID, 1000) / 1000.0f, mod((fragInstanceID+33)*3.1416, 100) / 100, mod((fragInstanceID+66)*1.618, 1000) / 1000, 1);
}