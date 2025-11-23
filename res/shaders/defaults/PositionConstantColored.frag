#version 450

//----------------------outputs----------------------
layout(location = 0) out vec4 outColor;


//-------------------push constants---------------------

layout(push_constant) uniform Constants
{
	vec4 color;
} color_constant;

void main() {
	outColor = color_constant.color;
}