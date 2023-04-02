#pragma once

#include "vulkan/vulkan.h"
#include "core/resource/raytracing/AccelerationStructure.h"
namespace HBE {
	class VK_Device;

	class VK_Buffer;
	class Mesh;
	class VK_AABBBottomLevelAccelerationStructure :public AABBAccelerationStructure{
		VkAccelerationStructureKHR handle;
		VK_Device *device;
		VK_Buffer *buffer;
		VK_Buffer *aabb_positions_buffer;
		VkDeviceOrHostAddressConstKHR address;
	public:
		VK_AABBBottomLevelAccelerationStructure(VK_Device *device,AABBAccelerationStructureInfo info);

		VkAccelerationStructureKHR getHandle() const;
		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
		~VK_AABBBottomLevelAccelerationStructure();

	};
}



