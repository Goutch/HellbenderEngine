#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_Fence {

		VkDevice device_handle = VK_NULL_HANDLE;
		VkFence handle = VK_NULL_HANDLE;
	public:
		VK_Fence(const VK_Device &device);
		VK_Fence(const VK_Fence &&other);
		~VK_Fence();
		void wait() const;
		void reset() const;
		const VkFence &getHandle() const;
	};

}

