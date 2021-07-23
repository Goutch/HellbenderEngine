#pragma once

#include "vector"
#include "vulkan/vulkan.h"


namespace HBE {
	class VK_Device;

	class VK_Swapchain;

	class VK_CommandPool {
		mutable uint32_t current = -1;
		VkCommandPool handle;
		const VK_Device *device;
		std::vector<VkCommandBuffer> command_buffers;

	public:
		~VK_CommandPool();

		VK_CommandPool(const VK_Device *device, int command_buffers_count);

		void begin(uint32_t i) const;

		void end(uint32_t i) const;

		const std::vector<VkCommandBuffer> &getBuffers() const;

		const VkCommandBuffer &getCurrentBuffer() const;

		void clear();

		void createCommandBuffers(int count);

		void reset(uint32_t i);

		const VkCommandPool &getHandle() const;
	};
}
