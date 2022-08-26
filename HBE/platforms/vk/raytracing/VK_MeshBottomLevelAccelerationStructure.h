#pragma once

#include "core/resource/raytracing/AccelerationStructure.h"
#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_MeshBottomLevelAccelerationStructure : public MeshAccelerationStructure {
		VkAccelerationStructureKHR handle;
		VK_Device *device;
		VK_Buffer *buffer;
	public:
		VK_MeshBottomLevelAccelerationStructure(VK_Device *device, MeshAccelerationStructureInfo info);
		VkAccelerationStructureKHR getHandle() const;

		~VK_MeshBottomLevelAccelerationStructure();
	};
}