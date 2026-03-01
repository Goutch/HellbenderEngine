//
// Created by User on 2021-10-31.
//

#include "VK_Queue.h"
#include "VK_Fence.h"
#include "core/utility/Log.h"
#include "VK_Device.h"
#include "VK_CommandPool.h"

namespace HBE
{
    VK_Queue::VK_Queue(VK_Device* device, QUEUE_FAMILY family, uint32_t family_index)
    {
        this->device_handle = device->getHandle();
        vkGetDeviceQueue(device_handle, family_index, 0, &handle);
        this->family_index = family_index;
        this->queue_family = family;
        command_pool.init(*device, 1, *this);
    }


    void VK_Queue::submit(VkCommandBuffer const& command_buffer,
                          VkFence fence,
                          VkSemaphore* wait,
                          VkPipelineStageFlags* wait_stage,
                          uint32_t wait_count,
                          VkSemaphore* signal,
                          uint32_t signal_count) const
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = wait_count;
        submitInfo.pWaitSemaphores = wait;
        submitInfo.pWaitDstStageMask = wait_stage;

        submitInfo.signalSemaphoreCount = signal_count;
        submitInfo.pSignalSemaphores = signal;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer;
        VkResult result = vkQueueSubmit(handle, 1, &submitInfo, fence);
        if (result != VK_SUCCESS)
        {
            Log::error("failed to submit draw command buffer!");
        }
    }

    const VkQueue& VK_Queue::getHandle() const
    {
        return handle;
    }

    void VK_Queue::wait() const
    {
        vkQueueWaitIdle(handle);
    }

    void VK_Queue::beginCommand()
    {
        command_pool.begin();
    }

    void VK_Queue::endCommand()
    {
        command_pool.end();
    }

    VK_Fence& VK_Queue::submitCommand()
    {
        return command_pool.submit(*this);
    }

    VK_CommandPool* VK_Queue::getCommandPool()
    {
        return &command_pool;
    }

    VK_Queue::~VK_Queue()
    {
        command_pool.release();
    }

    uint32_t VK_Queue::getFamilyIndex() const
    {
        return family_index;
    }
}
