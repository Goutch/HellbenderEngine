#pragma once

#include "PipelineInstance.h"
#include "Core.h"

namespace HBE
{
	class ComputePipeline;

	typedef uint32_t COMPUTE_INSTANCE_FLAGS;

	enum COMPUTE_INSTANCE_FLAG
	{
		COMPUTE_INSTANCE_FLAG_NONE = 0,
	};

	struct ComputeInstanceInfo
	{
		ComputePipeline* compute_pipeline;
		COMPUTE_INSTANCE_FLAGS flags = COMPUTE_INSTANCE_FLAG_NONE;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
		UniformMemoryInfo* uniform_memory_type_infos; //optional, specify memory type for specific uniform buffers
		uint32_t uniform_memory_type_info_count = 0;
		bool use_per_frame_uniform_buffers = true;
	};

	class HB_API ComputeInstance : public IPipelineInstance
	{
	public:

	};
}
