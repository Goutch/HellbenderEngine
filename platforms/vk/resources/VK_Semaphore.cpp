//
// Created by user on 5/24/2021.
//

#include "VK_Semaphore.h"

#include "HBE/platforms/vk/VK_Context.h"
#include "HBE/platforms/vk/VK_Device.h"
#include "core/utility/Log.h"

namespace HBE {
    void VK_Semaphore::alloc(VK_Context &context) {
        this->context = &context;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(context.device.getHandle(), &semaphoreInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create semaphore!");
        }
    }

    bool VK_Semaphore::allocated() {
        return handle != VK_NULL_HANDLE;
    }

    void VK_Semaphore::release() {
        vkDestroySemaphore(context->device.getHandle(), handle, nullptr);
    }

    const VkSemaphore &VK_Semaphore::getHandle() const {
        return handle;
    }
}
