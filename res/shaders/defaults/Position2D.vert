#version 450

//--------------------inputs--------------------
layout(location = 0) in vec2 inPosition;

//----------------------uniforms----------------------
layout(binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 projection;
} ubo;

layout(push_constant) uniform Constants
{
	mat4 transform;
} constants;

void main() {
	vec4 pos = constants.transform * vec4(inPosition, 0.0, 1.0);
	gl_Position = ubo.projection * ubo.view * pos;
}