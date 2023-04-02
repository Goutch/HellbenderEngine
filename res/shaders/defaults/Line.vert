#version 450
//binding=0 vertices
layout(location = 0) in vec2 inPosition;


layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

void main() {
    gl_Position = ubo.projection * ubo.view *vec4(inPosition, 0, 1.0);
}