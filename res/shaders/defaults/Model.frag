#version 450

layout(location = 0) out vec4 outColor;
layout (location = 0) in vec3 fragPos;
//layout(location = 1) in vec2 fragTexCoord;
layout(location = 1) in vec2 normal;

layout(binding = 1) uniform MaterialUniform {
    vec4 color;
    bool hasTexture;
} material;

void main() {
    if (material.hasTexture) {
        outColor = texture(sampler2D(texture), fragTexCoord);
    } else {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * vec3(1, 1, 1);
        vec3 lightPos=vec3(0, 0, 0);
        vec3 lightDir = normalize(lightPos- fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * material.color.rgb;
        outColor =vec4(result, 1.0);
    }
}