#pragma once

#include "vector"
#include "vulkan/vulkan.h"
#include "VK_Fence.h"
#include "VK_Queue.h"
#include "memory"

namespace HBE {
	class VK_Device;

	class VK_Swapchain;

	class VK_CommandPool {
		mutable int32_t current = 0;
		VkCommandPool handle;
		VK_Device &device;
		std::vector<VkCommandBuffer> command_buffers;
		std::vector<VK_Fence *> fences;
		void begin(uint32_t i) const;

		void end(uint32_t i) const;
		void reset(uint32_t i);
	public:
		~VK_CommandPool();

		VK_CommandPool(VK_Device &device, int command_buffers_count, const VK_Queue &queue);

		void begin() const;
		void end() const;

		const std::vector<VkCommandBuffer> &getBuffers() const;

		const VkCommandBuffer &getCurrentBuffer() const;

		void clear();

		void createCommandBuffers(int count);


		const VkCommandPool &getHandle() const;
		void submit(HBE::QUEUE_FAMILY queue, VkSemaphore *wait = nullptr,
					VkPipelineStageFlags *wait_stage = nullptr,
					uint32_t wait_count = 0,
					VkSemaphore *signal = nullptr,
					uint32_t signal_count = 0);


		VK_Fence &getCurrentFence();

	};
}
