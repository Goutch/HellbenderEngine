#pragma once

#include "vulkan/vulkan.h"
#include "vector"

namespace HBE {
	class VK_Device;

	class VK_Fence;

	enum QUEUE_FAMILY {
		QUEUE_FAMILY_GRAPHICS,
		QUEUE_FAMILY_COMPUTE,
		QUEUE_FAMILY_PRESENT,
		QUEUE_FAMILY_TRANSFER,
	};

	class VK_Queue {
	private:
		VkQueue handle;
		VK_Device *device;
	public:
		VK_Queue(VK_Device *device, uint32_t family_index);

		const VkQueue &getHandle() const;
		void wait() const;

		void submit(VkCommandBuffer const &command_buffer,
					VkFence fence = VK_NULL_HANDLE,
					VkSemaphore *wait = nullptr,
					VkPipelineStageFlags *wait_stage = nullptr,
					uint32_t wait_count = 0,
					VkSemaphore *signal = nullptr,
					uint32_t signal_count = 0) const;
	};
}



