//
// Created by User on 2021-10-31.
//

#include "VK_Queue.h"
#include "VK_Fence.h"
#include "core/utility/Log.h"
#include "VK_Device.h"

namespace HBE {


    VK_Queue::VK_Queue(VK_Device *device, uint32_t family_index, uint32_t max_opp_in_flight) {
        this->max_opp_in_flight = max_opp_in_flight;
        fences.resize(max_opp_in_flight);
        for (int i = 0; i < max_opp_in_flight; ++i) {
            fences[i] = new VK_Fence(*device);
        }

        vkGetDeviceQueue(device->getHandle(), family_index, 0, &handle);
    }


    void VK_Queue::submit(VkCommandBuffer const &command_buffer,
                          VkSemaphore *wait,
                          VkPipelineStageFlags *wait_stage,
                          uint32_t wait_count,
                          VkSemaphore *signal,
                          uint32_t signal_count) const {
        fences[current_opp]->wait();
        fences[current_opp]->reset();
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = wait_count;
        submitInfo.pWaitSemaphores = wait;
        submitInfo.pWaitDstStageMask = wait_stage;

        submitInfo.signalSemaphoreCount = signal_count;
        submitInfo.pSignalSemaphores = signal;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer;

        if (vkQueueSubmit(handle, 1, &submitInfo,
                          fences[current_opp]->getHandle()) != VK_SUCCESS) {
            Log::error("failed to submit draw command buffer!");
        }
        current_opp++;
    }

    const VkQueue &VK_Queue::getHandle() const {
        return handle;
    }

    void VK_Queue::wait() const {
        vkQueueWaitIdle(handle);
    }


}
