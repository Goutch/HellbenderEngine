#pragma once

#include "Core.h"
#include "core/Graphics.h"
#include "PipelineInstance.h"
namespace HBE {
	class HB_API ComputePipeline {
		Context &context;
		ComputePipelineHandle handle = HBE_NULL_HANDLE;
	public:
		ComputePipeline();

		~ComputePipeline();

		explicit ComputePipeline(const ComputePipelineInfo &info);

		void alloc(const ComputePipelineInfo &info);

		void allocInstance(PipelineInstance &instance);

		void release();

		ComputePipelineHandle getHandle();

		uint32_t getBinding(const char *name);

	};
}
