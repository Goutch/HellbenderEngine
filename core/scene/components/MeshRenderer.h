#pragma once
#include "Core.h"
#include "core/scene/ecs/Component.h"
#include "core/interface/PipelineInstanceInterface.h"

namespace HBE
{
    class Mesh;
    class RasterizationPipelineInstance;
    struct PushConstantInfo;

    struct HB_API MeshRenderer
    {
        COMPONENT_IDS(MeshRenderer);

        MeshHandle mesh = HBE_NULL_HANDLE;
        PipelineInstanceHandle pipeline_instance = HBE_NULL_HANDLE;
        RasterizationPipelineHandle rasterization_pipeline = HBE_NULL_HANDLE;
        PushConstantInfo* push_constants = nullptr;
        uint32_t push_constants_count = 0;
        uint32_t layer = 0;
        bool ordered = false;
        bool active = true;
        bool use_transform_matrix_as_push_constant = true;
    };
}
