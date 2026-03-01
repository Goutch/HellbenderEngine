#pragma once

#include "core/resource/raytracing/AccelerationStructure.h"
#include "platforms/vk/VK_Buffer.h"
#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Context;

	class VK_Buffer;

	class VK_MeshBottomLevelAccelerationStructure : public MeshAccelerationStructure {
		VkAccelerationStructureKHR handle;
		VK_Context *context;
		VK_Buffer buffer;
		VkDeviceOrHostAddressConstKHR address;
	public:
		VK_MeshBottomLevelAccelerationStructure(VK_Context *context, MeshAccelerationStructureInfo info);
		VkAccelerationStructureKHR getHandle() const;

		~VK_MeshBottomLevelAccelerationStructure();

		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
	};
}