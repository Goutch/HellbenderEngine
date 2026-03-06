#include "RasterizationPipeline.h"

#include "core/Application.h"
#include "core/Context.h"

namespace HBE
{
    RasterizationPipeline::RasterizationPipeline(const RasterizationPipelineInfo& info) : context(Application::instance->getContext())
    {
        alloc(info);
    }

    RasterizationPipeline::RasterizationPipeline() : context(Application::instance->getContext())
    {
    }

    RasterizationPipeline::~RasterizationPipeline()
    {
        release();
    }

    RasterizationPipelineHandle RasterizationPipeline::getHandle()
    {
        return handle;
    }

    void RasterizationPipeline::alloc(const RasterizationPipelineInfo& info)
    {
        context.createRasterizationPipeline(handle, info);
    }

    void RasterizationPipeline::allocInstance(PipelineInstance& instance)
    {
        PipelineInstanceInfo info;
        info.flags = PIPELINE_INSTANCE_FLAG_NONE;
        info.type = PIPELINE_INSTANCE_TYPE_RASTERIZATION;
        info.pipeline_handle = handle;
        info.preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
        instance.alloc(info);
    }

    void RasterizationPipeline::release()
    {
        if (handle != HBE_NULL_HANDLE)
            context.releaseRasterizationPipeline(handle);
        handle = HBE_NULL_HANDLE;
    }
}
