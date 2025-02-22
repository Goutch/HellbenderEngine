#pragma once

#include "Core.h"
#include "../IPipelineInstance.h"

namespace HBE
{
	class RaytracingPipeline;

	class RootAccelerationStructure;

	typedef uint32_t RAYTRACING_PIPELINE_INSTANCE_FLAGS;

	enum RAYTRACING_PIPELINE_INSTANCE_FLAG
	{
		RAYTRACING_PIPELINE_INSTANCE_FLAG_NONE = 0,
		RAYTRACING_PIPELINE_INSTANCE_FLAG_EMPTY_DESCRIPTOR_ALLOWED = 1,
	};

	struct RaytracingPipelineInstanceInfo
	{
		RaytracingPipeline* raytracing_pipeline = nullptr;
		RAYTRACING_PIPELINE_INSTANCE_FLAGS flags = RAYTRACING_PIPELINE_INSTANCE_FLAG_NONE;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
		UniformMemoryInfo* uniform_memory_type_infos; //optional, specify memory type for specific uniform buffers
		uint32_t uniform_memory_type_info_count = 0;
	};

	class HB_API RaytracingPipelineInstance : public IPipelineInstance
	{
	public:
		virtual void setAccelerationStructure(uint32_t binding, const RootAccelerationStructure* accelerationStructure, int32_t frame = -1) = 0;

		virtual void setAccelerationStructure(const std::string& name, const RootAccelerationStructure* accelerationStructure, int32_t frame = -1) = 0;
	};
}
