#pragma once

#include "Core.h"
#include "core/resource/PipelineInstance.h"

namespace HBE
{
	class HB_API RaytracingPipeline
	{
		RaytracingPipelineHandle handle;
		Context& context;
	public:
		RaytracingPipeline();
		~RaytracingPipeline();
		void alloc(const RaytracingPipelineInfo &info);
		void allocInstance(PipelineInstance &instance) const;
		void release();
		RaytracingPipelineHandle getHandle();
		RaytracingPipelineHandle &getHandleRef();
		bool allocated();

	};
}
