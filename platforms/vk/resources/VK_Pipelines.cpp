#include "VK_RasterizationPipeline.h"
#include "raytracing/VK_RaytracingPipeline.h"
#include "../VK_Pipelines.h"
#include "../VK_Context.h"
#define RASTER_PIPELINE_FUNC_HEADER \
VK_Context& context = *static_cast<VK_Context*>(context_ptr);\
VK_Pipelines& pipelines = context.pipelines;\
HandleProvider& provider = pipelines.rasterization_pipeline_handle_provider;
#define SHADER_FUNC_HEADER \


namespace HBE
{
    void VK_Pipelines::init(VK_Context* context)
    {
        this->context = context;
    }

    void VK_Pipelines::release()
    {
        for (VK_Shader& data : shader_data)
        {
            if (data.allocated())
            {
                data.release();
                Log::warning("A Shader was not released properly");
            }
        }
        for (VK_RasterizationPipeline& data : rasterization_pipelines_data)
        {
            if (data.allocated())
            {
                data.release();
                Log::warning("A Rasterization pipeline was not released properly");
            }
        }

        for (VK_RaytracingPipeline& data : raytracing_pipelines_data)
        {
            if (data.allocated())
            {
                data.release();
                Log::warning("A Raytracing pipeline was not released properly");
            }
        }
        for (VK_ComputePipeline& data : compute_pipelines_data)
        {
            if (data.allocated())
            {
                data.release();
                Log::warning("A Compute pipeline was not released properly");
            }
        }
    }

    HBE_RESULT VK_Pipelines::createShader(ShaderHandle& handle, const ShaderInfo& shader_info)
    {
        handle = shader_handle_provider.create();
        shader_data.resize(shader_handle_provider.size());
        shader_data[provider.index(handle)].alloc(context, shader_info);
        return HBE_RESULT_SUCCESS;;
    }


    HBE_RESULT VK_Pipelines::releaseShader(ShaderHandle handle)
    {
        SHADER_FUNC_HEADER
        if (!provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        provider.release(handle);
        pipelines.shader_data[provider.index(handle)].release();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Pipelines::createRasterizationPipeline(RasterizationPipelineHandle& handle, const RasterizationPipelineInfo& raster_pipeline_info)
    {
        handle = provider.create();
        pipelines.rasterization_pipelines_data.resize(provider.size());
        pipelines.rasterization_pipelines_data[provider.index(handle)].alloc(context, raster_pipeline_info);
    }

    HBE_RESULT VK_Pipelines::releaseRasterizationPipeline(RasterizationPipelineHandle handle)
    {
        if (!provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        provider.release(handle);
        pipelines.rasterization_pipelines_data[provider.index(handle)].release();
    }
}
