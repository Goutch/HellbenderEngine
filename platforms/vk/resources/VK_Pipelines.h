#pragma once
#include "HandleProvider.h"
#include "../VK_ComputePipeline.h"
#include "core/interface/RaytracingPipelineInterface.h"

namespace HBE
{
    class VK_RaytracingPipeline;
    class VK_Context;

    class VK_Pipelines
    {
        VK_Context* context;

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
        void init(VK_Context* context);
        const VK_Shader* getShader(ShaderHandle handle) { return &shader_data[shader_handle_provider.index(handle)]; }
        void release();
        inline HBE_RESULT createShader(ShaderHandle& handle, const ShaderInfo& shader_info);

        inline HBE_RESULT releaseShader(ShaderHandle handle);

        inline HBE_RESULT createRasterizationPipeline(RasterizationPipelineHandle& handle, const RasterizationPipelineInfo& raster_pipeline_info);

        inline HBE_RESULT releaseRasterizationPipeline(RasterizationPipelineHandle handle);



    };
}
