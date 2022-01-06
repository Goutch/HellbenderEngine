#version 450
//binding=0 vertices
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

//binding=1 instanced
layout(location = 2) in mat4 inInstanceTransforms;
layout(location = 3) out vec3 worldPosition;

layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

void main() {
    worldPosition = (inInstanceTransforms * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.projection * ubo.view * vec4(worldPosition, 1.0);
    fragTexCoord = inTexCoord;
}