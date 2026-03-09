#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE
{



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
