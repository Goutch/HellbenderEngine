#pragma once

#include "Core.h"
#include "core/Graphics.h"
namespace HBE {
	class HB_API ComputePipeline {
		Context &context;
		ComputePipelineHandle handle = HBE_NULL_HANDLE;
	public:
		ComputePipeline();

		~ComputePipeline();

		explicit ComputePipeline(const RasterizationPipelineInfo &info);

		void alloc(const ComputePipelineInfo &info);

		void allocInstance(PipelineInstance &instance);

		void release();

		ComputePipelineHandle getHandle();

		uint32_t getBinding(const char *name);

	};
}
