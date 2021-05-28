//
// Created by user on 5/24/2021.
//

#include "VK_Semaphore.h"
#include "VK_Device.h"
#include "core/utility/Log.h"
namespace HBE{
    HBE::VK_Semaphore::VK_Semaphore(const VK_Device& device) {
        this->device=&device;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if( vkCreateSemaphore(device.getHandle(), &semaphoreInfo, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("failed to create semaphore!");
        }
    }

    VK_Semaphore::~VK_Semaphore() {
        vkDestroySemaphore(device->getHandle(), handle, nullptr);
    }

    const VkSemaphore &VK_Semaphore::getHandle() {
        return handle;
    }
}
