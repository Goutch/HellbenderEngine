#version 450

layout (location = 0) out vec4 outColor;

layout (location = 1) in vec2 fragTexCoord;

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
        outColor = material.color* smoothstep(1, 0, (dist - 0.40) / 0.10);
    }
    else {
        outColor = vec4(0, 0, 0, 0);//material.color * ((dist * 2));
    }

}