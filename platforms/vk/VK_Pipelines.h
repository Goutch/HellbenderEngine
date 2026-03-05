#pragma once
#include "HandleProvider.h"
#include "VK_ComputePipeline.h"
#include "VK_RasterizationPipeline.h"
#include "raytracing/VK_RaytracingPipeline.h"

namespace HBE {
    class VK_Pipelines {
        HandleProvider handle_provider;
        std::vector<VK_RasterizationPipeline> rasterization_pipelines_data;
        RawVector<RasterizationPipelineInfo> rasterization_creation_infos;
        std::vector<VK_ComputePipeline> compute_pipelines_data;
        RawVector<ComputePipelineInfo> compute_creation_infos;
        std::vector<VK_RaytracingPipeline> raytracing_pipelines_data;
        RawVector<RaytracingPipelineInfo> raytracing_creation_infos;

    };
}
