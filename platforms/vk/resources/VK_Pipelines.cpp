SHADER_FUNC_HEADER //
// Created by username on 3/4/2026.
//



#include "../VK_Pipelines.h"

#include "../VK_Context.h"
#include "dependencies/glfw/src/internal.h"

#define RASTER_PIPELINE_FUNC_HEADER \
VK_Context& context = *static_cast<VK_Context*>(context_ptr);\
VK_Pipelines& pipelines = context.pipelines;\
HandleProvider& provider = pipelines.rasterization_pipeline_handle_provider;
#define SHADER_FUNC_HEADER \
VK_Context& context = *static_cast<VK_Context*>(context_ptr);\
VK_Pipelines& pipelines = context.pipelines;\
HandleProvider& provider = pipelines.shader_handle_provider;

namespace HBE {
    HBE_RESULT VK_Pipelines::createShader(void *context_ptr, ShaderHandle &handle, const ShaderInfo &shader_info) {
        SHADER_FUNC_HEADER
        handle = provider.create();
        pipelines.shader_data[provider.index(handle)].alloc(shader_info);
        return HBE_RESULT_SUCCESS;;
    }

    HBE_RESULT VK_Pipelines::releaseShader(void *context_ptr, ShaderHandle handle) {
        SHADER_FUNC_HEADER
        if (!provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        provider.release(handle);
        pipelines.shader_data[provider.index(handle)].release();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Pipelines::createRasterizationPipeline(void *context_ptr, RasterizationPipelineHandle &handle, const RasterizationPipelineInfo &raster_pipeline_info) {
        RASTER_PIPELINE_FUNC_HEADER
        handle = provider.create();
        pipelines.rasterization_pipelines_data.resize(provider.size());
        pipelines.rasterization_pipelines_data[provider.index(handle)].alloc(raster_pipeline_info);
    }

    HBE_RESULT VK_Pipelines::releaseRasterizationPipeline(void *context_ptr, RasterizationPipelineHandle handle) {
        RASTER_PIPELINE_FUNC_HEADER
        if (!provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        provider.release(handle);
        pipelines.rasterization_pipelines_data[provider.index(handle)].release();
    }
}
