#pragma once

#include "Core.h"
#include "../Shader.h"

namespace HBE {
	struct RaytracingPipelineInfo {
		Shader* raygen_shader;
		Shader* miss_shader;
		Shader* closest_hit_shader;
	};

	class HB_API RaytracingPipeline : public Resource {

	};
}
