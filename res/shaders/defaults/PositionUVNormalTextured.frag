#version 450
//----------------------inputs----------------------
layout (location = 0) in vec3 fragmentPos;
layout (location = 1) in vec2 fragmentUV;
layout (location = 2) in vec3 fragmentNormal;

//----------------------outputs----------------------
layout (location = 0) out vec4 outColor;


//----------------------uniforms----------------------
layout (binding = 1) uniform sampler2D albedo;

layout (binding = 2) uniform MaterialProperties {
	vec4 base_color;
	vec4 emmisive_factor;
	float metal_factor;
	float roughnes;
	uint has_albedo;
	uint has_metallic_roughness;
	uint has_emmisive;
	uint has_normal;
	uint has_occlusion;
} material;

void main() {
	if (material.has_albedo == 1)
	{
		//outColor = vec4(mod((fragmentUV+vec2(1))/2.0f, vec2(1.0)), 0.0, 1.0)/2.0;
		outColor = texture(albedo, fragmentUV);
	}
	else
	{
		outColor = material.base_color;
	}

	if(outColor.a < 0.01)
		discard;
}