#pragma once

#include "vulkan/vulkan.h"
#include "dependencies/utils-collection/Event.h"
#include "core/graphics/Fence.h"

namespace HBE {
	class VK_Device;

	class VK_Fence : public Fence {

		VkDevice device_handle = VK_NULL_HANDLE;
		VkFence handle = VK_NULL_HANDLE;

	public:
		VK_Fence(const VK_Device &device);
		VK_Fence(const VK_Fence &&other);
		~VK_Fence();
		void wait() const override;
		void reset() const;
		bool isSet() const;
		const VkFence &getHandle() const;
	};

}

