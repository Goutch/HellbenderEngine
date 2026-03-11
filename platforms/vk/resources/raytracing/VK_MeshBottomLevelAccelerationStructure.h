#pragma once

#include "core/interface/MeshAccelerationStructureInterface.h"
#include "platforms/vk/VK_Buffer.h"
#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Context;

	class VK_Buffer;

	class VK_MeshBottomLevelAccelerationStructure {
		VkAccelerationStructureKHR handle;
		VK_Context *context;
		VK_Buffer buffer;
		VkDeviceOrHostAddressConstKHR address;
	public:
		VK_MeshBottomLevelAccelerationStructure() = default;
		void alloc(VK_Context *context, MeshAccelerationStructureInfo info);
		void release();
		bool allocated();
		VkAccelerationStructureKHR getHandle() const;

		~VK_MeshBottomLevelAccelerationStructure();

		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
	};
}