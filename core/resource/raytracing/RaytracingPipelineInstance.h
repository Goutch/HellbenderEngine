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
		RAYTRACING_PIPELINE_INSTANCE_FLAGS flags = RAYTRACING_INSTANCE_FLAG_NONE;
	};

	class HB_API RaytracingPipelineInstance : public IPipelineInstance {
	public:
		virtual void setAccelerationStructure(uint32_t binding, const RootAccelerationStructure *accelerationStructure, int32_t frame = -1) = 0;

		virtual void setAccelerationStructure(const std::string &name, const RootAccelerationStructure *accelerationStructure, int32_t frame = -1) = 0;
	};
}