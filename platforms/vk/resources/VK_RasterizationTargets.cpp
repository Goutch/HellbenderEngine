//
// Created by username on 3/6/2026.
//

#include "VK_RasterizationTargets.h"

#include "platforms/vk/VK_Context.h"

namespace HBE {
    HBE_RESULT VK_RasterizationTargets::createRasterizationTarget(void *context_ptr, RasterizationTargetHandle &handle, RasterizationTargetInfo info) {


        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_RasterizationTargets::releaseRasterizationTarget(void *context_ptr, RasterizationTargetHandle handle) {
        VK_Context& context = *static_cast<VK_Context*>(context_ptr);
        VK_RasterizationTargets& instance = context->rasterization_targets;
        HandleProvider& handle_provider = instance.handle_provider;
        std::vector<VK_RenderPass>& rasterization_targets = instance.rasterization_targets;
        if (!handle_provider.valid(handle))
            return HBE_RESULT_INVALID_HANDLE;
        rasterization_targets[handle_provider.index(handle)].release();
        handle_provider.release(handle);


        return HBE_RESULT_SUCCESS;
    }
}
