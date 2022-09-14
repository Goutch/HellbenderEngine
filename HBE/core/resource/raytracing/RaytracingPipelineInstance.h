#pragma once

#include "Core.h"
#include "../IPipelineInstance.h"

namespace HBE {
	class RaytracingPipeline;

	class RootAccelerationStructure;

	typedef uint32_t RAYTRACING_PIPELINE_INSTANCE_FLAGS;
	enum RAYTRACING_PIPELINE_INSTANCE_FLAG {
		RAYTRACING_INSTANCE_FLAG_NONE = 0,
	};
	struct RaytracingPipelineInstanceInfo {
		RaytracingPipeline *raytracing_pipeline = nullptr;
		RootAccelerationStructure *root_acceleration_structure = nullptr;
		RAYTRACING_PIPELINE_INSTANCE_FLAGS flags = RAYTRACING_INSTANCE_FLAG_NONE;
	};

	class HB_API RaytracingPipelineInstance : public IPipelineInstance {

	};
}