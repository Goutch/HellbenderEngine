#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_BottomLevelAccelerationStructure {

		VkAccelerationStructureKHR handle;
		VK_Device &device;
	public:
		VK_BottomLevelAccelerationStructure(VK_Device &device);

		VkAccelerationStructureKHR getHandle() const;

		~VK_BottomLevelAccelerationStructure();
	};
}



