//
// Created by User on 2021-10-31.
//

#include "VK_Queue.h"
#include "VK_Fence.h"
#include "core/utility/Log.h"
#include "VK_Device.h"

namespace HBE {


	VK_Queue::VK_Queue(VK_Device *device, uint32_t family_index) {
		vkGetDeviceQueue(device->getHandle(), family_index, 0, &handle);
	}


	void VK_Queue::submit(VkCommandBuffer const &command_buffer,
						  VkFence fence,
						  VkSemaphore *wait,
						  VkPipelineStageFlags *wait_stage,
						  uint32_t wait_count,
						  VkSemaphore *signal,
						  uint32_t signal_count) const {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = wait_count;
		submitInfo.pWaitSemaphores = wait;
		submitInfo.pWaitDstStageMask = wait_stage;

		submitInfo.signalSemaphoreCount = signal_count;
		submitInfo.pSignalSemaphores = signal;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_buffer;
		VkResult result=vkQueueSubmit(handle, 1, &submitInfo,fence);
		if (result != VK_SUCCESS) {
			Log::error("failed to submit draw command buffer!");
		}
	}

	const VkQueue &VK_Queue::getHandle() const {
		return handle;
	}

	void VK_Queue::wait() const {
		vkQueueWaitIdle(handle);
	}


}
