#version 450
//binding=0 vertices
layout(location = 0) in vec3 inPosition;

//binding=1 instanced
layout(location = 1) in mat4 inInstanceTransform;

layout(location = 2) out vec3 worldPosition;
layout(location = 3) out flat mat4 fragInstanceTransform;


layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

void main() {
    worldPosition = (inInstanceTransform * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.projection * ubo.view * vec4(worldPosition, 1.0);
    fragInstanceTransform=inInstanceTransform;
}