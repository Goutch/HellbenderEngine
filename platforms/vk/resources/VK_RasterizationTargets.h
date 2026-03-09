#pragma once
#include "HBETypes.h"
#include "core/interface/RasterizationTargetInterface.h"
#include "platforms/vk/VK_RenderPass.h"

namespace HBE {
    class VK_RasterizationTargets {
        std::vector<VK_RenderPass> rasterization_targets;
        RawVector<RasterizationTargetInfo> creation_infos;
        HandleProvider handle_provider;
    public:
        static HBE_RESULT createRasterizationTarget(void* context_ptr,RasterizationTargetHandle& handle,RasterizationTargetInfo info);
        static HBE_RESULT releaseRasterizationTarget(void* context_ptr,RasterizationTargetHandle handle);
    };
}
