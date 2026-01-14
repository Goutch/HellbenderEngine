#pragma once
#include "Core.h"
#include "core/scene/ecs/Component.h"

namespace HBE {
    class Mesh;
    class RasterizationPipelineInstance;
    class PushConstantInfo;

    struct HB_API MeshRenderer {
        COMPONENT_IDS(MeshRenderer);

        Mesh *mesh = nullptr;
        RasterizationPipelineInstance *pipeline_instance = nullptr;
        uint32_t layer = 0;
        bool ordered = false;
        bool active = true;
        bool use_transform_matrix_as_push_constant = true;
        PushConstantInfo *push_constants = nullptr;
        uint32_t push_constants_count = 0;
    };
}
