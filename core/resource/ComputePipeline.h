#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE
{
	class Shader;

	class Image;

	class ComputeBuffer;

	class RasterizationTarget;

	typedef uint32_t COMPUTE_PIPELINE_FLAG;

	enum COMPUTE_PIPELINE_FLAGS
	{
		COMPUTE_PIPELINE_FLAG_NONE = 0,
		COMPUTE_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR = 1,
	};

	struct ComputePipelineInfo
	{
		Shader* compute_shader;
		COMPUTE_PIPELINE_FLAGS flags = COMPUTE_PIPELINE_FLAG_NONE;
	};

	class HB_API ComputePipeline
	{
	protected:
		vec3i workgroup_size;

	public:
		virtual const vec3i& getWorkgroupSize() const = 0;

		virtual ~ComputePipeline() = default;

		virtual COMPUTE_PIPELINE_FLAGS getFlags() const = 0;
	};
}
