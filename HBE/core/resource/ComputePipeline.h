#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE {
	class Shader;

	class ComputeBuffer;

	typedef uint32_t COMPUTE_PIPELINE_FLAG;
	enum COMPUTE_PIPELINE_FLAGS {
		COMPUTE_PIPELINE_FLAG_NONE = 0,
	};
	struct ComputePipelineInfo {
		Shader *compute_shader;
		COMPUTE_PIPELINE_FLAG flags = COMPUTE_PIPELINE_FLAG_NONE;
	};

	class HB_API ComputePipeline : public Resource {

		virtual void updateBuffer(ComputeBuffer *buffer) = 0;

		virtual void dispatch(int x, int y, int z) = 0;

		virtual void wait() = 0;
	};
}
