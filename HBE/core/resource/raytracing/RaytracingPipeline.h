#pragma once

#include "Core.h"
#include "../Shader.h"

namespace HBE {
	struct RaytracingShaderGroup {
		int32_t closest_hit_shader_index = -1;
		///Optionnal
		int32_t any_hit_shader_index = -1;
		///Optionnal for triangle intersection
		int32_t intersection_shader_index = -1;
	};

	struct RaytracingPipelineInfo {
		Shader *raygen_shader;

		Shader **miss_shaders;
		uint32_t miss_shader_count;
		///Any hit and intersection shaders and closest hit
		Shader **hit_shaders;
		uint32_t hit_shader_count;

		RaytracingShaderGroup *shader_groups;
		uint32_t shader_group_count;

		uint32_t max_recursion_depth = 1;
	};

	class HB_API RaytracingPipeline : public Resource {

	};
}
