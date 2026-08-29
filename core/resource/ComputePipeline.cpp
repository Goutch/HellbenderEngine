
#include "ComputePipeline.h"
#include "core/Application.h"

namespace HBE{

	ComputePipeline::ComputePipeline(const ComputePipelineInfo &info) : context(*Application::instance->getContext()){
		alloc(info);
	}
	void ComputePipeline::alloc(const ComputePipelineInfo &info) {
		context.createComputePipeline(handle, info);
	}

	void ComputePipeline::allocInstance(PipelineInstance &instance) {
		PipelineInstanceInfo info{};
		info.pipeline_handle = handle;
		info.type = PIPELINE_INSTANCE_TYPE_COMPUTE;
		info.preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
		context.createPipelineInstance(instance.getHandleRef(), info);
	}

	void ComputePipeline::release() {
		if (handle != HBE_NULL_HANDLE)
			context.releaseRasterizationPipeline(handle);
		context.releaseComputePipeline(handle);
	}

	ComputePipelineHandle ComputePipeline::getHandle() {
		return handle;
	}

	uint32_t ComputePipeline::getBinding(const char *name) {
		return 0;
	}

	ComputePipeline::ComputePipeline()  : context(*Application::instance->getContext()) {
	}

	ComputePipeline::~ComputePipeline() {
		release();
	}

}