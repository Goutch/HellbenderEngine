#include "vulkan/vulkan.h"
#include "VK_Fence.h"

#include "VK_Context.h"
#include "VK_Device.h"
#include "core/utility/Log.h"

namespace HBE
{
    void VK_Fence::init(VK_Context* context)
    {
        this->context = context;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(context->device.getHandle(), &fenceInfo, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("failed to create fence!");
        }
    }

    void VK_Fence::release()
    {
        vkDestroyFence(context->device.getHandle(), handle, nullptr);
    }

    const VkFence& VK_Fence::getHandle() const
    {
        return handle;
    }

    VK_Fence::VK_Fence(VK_Fence&& other) noexcept
    {
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
        context = other.context;
        other.context = nullptr;
    }

    void VK_Fence::wait() const
    {
        if (vkWaitForFences(context->device.getHandle(), 1, &handle, VK_TRUE, UINT64_MAX) == VK_TIMEOUT)
        {
            Log::error("Timeout");
        }
    }

    void VK_Fence::reset() const
    {
        vkResetFences(context->device.getHandle(), 1, &handle);
    }

    bool VK_Fence::isSet() const
    {
        return vkGetFenceStatus(context->device.getHandle(), handle) == VK_SUCCESS;
    }
}
