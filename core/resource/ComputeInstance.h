#pragma once

#include "IPipelineInstance.h"
#include "Core.h"

namespace HBE {
	class ComputePipeline;

	typedef uint32_t COMPUTE_INSTANCE_FLAGS;
	enum COMPUTE_INSTANCE_FLAG {
		COMPUTE_INSTANCE_FLAG_NONE = 0,
	};
	struct ComputeInstanceInfo {
		ComputePipeline *compute_pipeline;
		COMPUTE_INSTANCE_FLAGS flags = COMPUTE_INSTANCE_FLAG_NONE;
	};

	class HB_API ComputeInstance : public IPipelineInstance {
	public:
		virtual const ComputePipeline *getComputePipeline() const = 0;
		virtual void dispatch(uint32_t group_count_x,
							  uint32_t group_count_y,
							  uint32_t group_count_z) = 0;
		virtual bool isDispatchFinished() = 0;
		virtual void wait() = 0;

	};

}
