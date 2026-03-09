#pragma once
#include "HandleProvider.h"
#include "../VK_ComputePipeline.h"
#include "VK_RasterizationPipeline.h"
#include "../raytracing/VK_RaytracingPipeline.h"

namespace HBE {
    class VK_RaytracingPipeline;
    struct RaytracingPipelineInfo;
    class Context;

    class VK_Pipelines {


        HandleProvider shader_handle_provider;
        std::vector<VK_Shader> shader_data;
        RawVector<ShaderInfo> shader_info;

        HandleProvider rasterization_pipeline_handle_provider;
        std::vector<VK_RasterizationPipeline> rasterization_pipelines_data;
        RawVector<RasterizationPipelineInfo> rasterization_creation_infos;

        HandleProvider compute_pipeline_handle_provider;
        std::vector<VK_ComputePipeline> compute_pipelines_data;
        RawVector<ComputePipelineInfo> compute_creation_infos;

        HandleProvider raytracing_pipeline_handle_provider;
        std::vector<VK_RaytracingPipeline> raytracing_pipelines_data;
        RawVector<RaytracingPipelineInfo> raytracing_creation_infos;

    public:
        static inline HBE_RESULT createShader(void *context_ptr, ShaderHandle &handle, const ShaderInfo &shader_info);

        static inline HBE_RESULT releaseShader(void *context_ptr, ShaderHandle handle);

        static inline HBE_RESULT createRasterizationPipeline(void* context_ptr,RasterizationPipelineHandle& handle,const RasterizationPipelineInfo& raster_pipeline_info);

        static inline HBE_RESULT releaseRasterizationPipeline(void* context_ptr,RasterizationPipelineHandle handle);
    };

}
