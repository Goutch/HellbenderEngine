#pragma once

#include "IPipelineInstance.h"
#include "Core.h"

namespace HBE {
	class ComputePipeline;

	typedef uint32_t COMPUTE_INSTANCE_FLAGS;
	enum COMPUTE_INSTANCE_FLAG {
		COMPUTE_INSTANCE_FLAG_NONE = 0,
		GRAPHIC_INSTANCE_FLAG_EMPTY_DESCRIPTOR_ALLOWED = 1,
	};
	struct ComputeInstanceInfo {
		ComputePipeline *compute_pipeline;
		COMPUTE_INSTANCE_FLAGS flags = COMPUTE_INSTANCE_FLAG_NONE;
	};

	class HB_API ComputeInstance : public IPipelineInstance {
	public:
		virtual const ComputePipeline *getComputePipeline() const = 0;
		virtual void dispatchAsync(uint32_t size_x,
		                           uint32_t size_y,
		                           uint32_t size_z) = 0;
		virtual bool isDispatchFinished() = 0;
		virtual void wait() = 0;

	};

}
