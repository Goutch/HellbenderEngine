#include "vulkan/vulkan.h"
#include "VK_Fence.h"
#include "VK_Device.h"
#include "core/utility/Log.h"

namespace HBE {
    VK_Fence::VK_Fence(const VK_Device &device) {
        this->device = &device;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(device.getHandle(), &fenceInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create fence!");
        }
    }

    VK_Fence::~VK_Fence() {
        vkDestroyFence(device->getHandle(), handle, nullptr);
    }

    const VkFence &VK_Fence::getHandle() {
        return handle;
    }

    void VK_Fence::wait() {
        vkWaitForFences(device->getHandle(), 1, &handle, VK_TRUE, UINT64_MAX);
    }

    void VK_Fence::reset() {
        vkResetFences(device->getHandle(), 1, &handle);
    }
}

