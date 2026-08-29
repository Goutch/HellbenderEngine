
#include "RaytracingPipeline.h"
#include "core/Application.h"

namespace HBE{
	RaytracingPipeline::RaytracingPipeline() : context(*Application::instance->getContext()) {

	}

	RaytracingPipeline::~RaytracingPipeline() {
		release();
	}

	void RaytracingPipeline::alloc(const RaytracingPipelineInfo &info) {
		context.createRaytracingPipeline(handle,info);
	}

	void RaytracingPipeline::release() {
		if(allocated())
		{
			context.releaseRaytracingPipeline(handle);
			handle = HBE_NULL_HANDLE;
		}
	}

	RaytracingPipelineHandle RaytracingPipeline::getHandle() {
		return handle;
	}

	RaytracingPipelineHandle &RaytracingPipeline::getHandleRef() {
		return handle;
	}

	bool RaytracingPipeline::allocated() {
		return handle != HBE_NULL_HANDLE;
	}

	void RaytracingPipeline::allocInstance(PipelineInstance &instance) const {
		PipelineInstanceInfo info{};
		info.flags = PIPELINE_INSTANCE_FLAG_NONE;
		info.type = PIPELINE_INSTANCE_TYPE_RAY_TRACING;
		info.pipeline_handle = handle;
		info.preferred_memory_type_flags = MEMORY_TYPE_FLAG_MAPPABLE;
		instance.alloc(info);
	}
}


