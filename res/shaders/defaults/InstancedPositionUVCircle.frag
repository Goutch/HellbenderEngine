#version 450

layout (location = 0) out vec4 outColor;

layout (location = 1) in vec2 fragTexCoord;

layout (location = 2) in flat int fragInstanceID;

layout (binding = 1) uniform MaterialUniform {
    vec4 color;
} material;


void main() {
    float dist = distance(fragTexCoord - vec2(0.5, 0.5), vec2(0, 0));
    if (dist > 0.5)
    {
        discard;
    }
    else if (dist > 0.40)
    {
        outColor = vec4(1, 1, 1, 1);
    }
    else {
        outColor = vec4(mod(fragInstanceID, 1000) / 1000.0f, mod((fragInstanceID+33)*3.1416, 100) / 100, mod((fragInstanceID+66)*1.618, 1000) / 1000, 1);
    }

}