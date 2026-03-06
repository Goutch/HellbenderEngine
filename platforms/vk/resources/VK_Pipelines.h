#pragma once
#include "HandleProvider.h"
#include "../VK_ComputePipeline.h"
#include "VK_RasterizationPipeline.h"
#include "../raytracing/VK_RaytracingPipeline.h"

namespace HBE {
    class Context;

    class VK_Pipelines {
        HandleProvider handle_provider;
        HandleProvider shader_handle_provider;
        std::vector<VK_Shader> shader_data;
        RawVector<ShaderInfo> shader_info;
        std::vector<VK_RasterizationPipeline> rasterization_pipelines_data;
        RawVector<RasterizationPipelineInfo> rasterization_creation_infos;
        std::vector<VK_ComputePipeline> compute_pipelines_data;
        RawVector<ComputePipelineInfo> compute_creation_infos;
        std::vector<VK_RaytracingPipeline> raytracing_pipelines_data;
        RawVector<RaytracingPipelineInfo> raytracing_creation_infos;

    public:
        HBE_RESULT createShader(void *context_ptr, ShaderHandle &handle, const ShaderInfo &shader_info);

        HBE_RESULT releaseShader(void *context_ptr, ShaderHandle handle);

        HBE_RESULT createRasterizationPipeline(void* context_ptr,RasterizationPipelineHandle& handle,const RasterizationPipelineInfo& raster_pipeline_info);
    };
}
