#include <core/utility/Profiler.h>
#include "../VK_Device.h"
#include "VK_AABBBottomLevelAccelerationStructure.h"
#include "../VK_Buffer.h"
#include "vulkan/vulkan.h"
#include "../VK_CommandPool.h"

namespace HBE
{
	VK_AABBBottomLevelAccelerationStructure::VK_AABBBottomLevelAccelerationStructure(VK_Device* device, AABBAccelerationStructureInfo info)
	{
		this->device = device;
		HB_PROFILE_BEGIN("Build AABB Acceleration Structure");


		VkAabbPositionsKHR aabb_positions{};
		aabb_positions.maxX = info.max.x;
		aabb_positions.maxY = info.max.y;
		aabb_positions.maxZ = info.max.z;
		aabb_positions.minX = info.min.x;
		aabb_positions.minY = info.min.y;
		aabb_positions.minZ = info.min.z;


		aabb_positions_buffer = new VK_Buffer(device,
		                                      static_cast<void*>(&aabb_positions),
		                                      static_cast<VkDeviceSize>(sizeof(VkAabbPositionsKHR)),
		                                      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
		                                      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
		                                      info.preferred_memory_type_flags);

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.buffer = aabb_positions_buffer->getHandle();

		VkDeviceOrHostAddressConstKHR aabb_position_buffer_address{};
		aabb_position_buffer_address.deviceAddress = device->vkGetBufferDeviceAddressKHR(device->getHandle(), &bufferDeviceAddressInfo);

		VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
		accelerationStructureGeometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
		accelerationStructureGeometry.geometry.aabbs.data = aabb_position_buffer_address;
		accelerationStructureGeometry.geometry.aabbs.stride = sizeof(VkAabbPositionsKHR);


		// Get size info
		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationStructureBuildGeometryInfo.geometryCount = 1;
		accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;


		VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
		accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;


		device->vkGetAccelerationStructureBuildSizesKHR(
			device->getHandle(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&accelerationStructureBuildGeometryInfo,
			&accelerationStructureBuildGeometryInfo.geometryCount,
			&accelerationStructureBuildSizesInfo);

		buffer = new VK_Buffer(device,
		                       accelerationStructureBuildSizesInfo.accelerationStructureSize,
		                       VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		                       info.preferred_memory_type_flags);


		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.buffer = buffer->getHandle();
		accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
		accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		device->vkCreateAccelerationStructureKHR(device->getHandle(), &accelerationStructureCreateInfo, nullptr, &handle);

		// Create a small scratch buffer used during build of the bottom level acceleration structure
		VK_Buffer scratchBuffer = VK_Buffer(device,
		                                    accelerationStructureBuildSizesInfo.buildScratchSize,
		                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		                                    MEMORY_TYPE_FLAG_GPU_LOCAL);

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = handle;
		accelerationBuildGeometryInfo.geometryCount = 1;
		accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
		accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress().deviceAddress;


		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = 1;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = {&accelerationStructureBuildRangeInfo};

		device->getQueue(QUEUE_FAMILY_GRAPHICS).beginCommand();
		device->vkCmdBuildAccelerationStructuresKHR(
			device->getQueue(QUEUE_FAMILY_GRAPHICS).getCommandPool()->getCurrentBuffer(),
			1,
			&accelerationBuildGeometryInfo,
			accelerationBuildStructureRangeInfos.data());
		device->getQueue(QUEUE_FAMILY_GRAPHICS).endCommand();
		device->getQueue(QUEUE_FAMILY_GRAPHICS).submitCommand().wait();


		VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
		accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		accelerationDeviceAddressInfo.accelerationStructure = handle;

		address.deviceAddress = device->vkGetAccelerationStructureDeviceAddressKHR(device->getHandle(), &accelerationDeviceAddressInfo);

		HB_PROFILE_END("Build AABB Acceleration Structure");
	}

	VK_AABBBottomLevelAccelerationStructure::~VK_AABBBottomLevelAccelerationStructure()
	{
		delete aabb_positions_buffer;
		delete buffer;
		device->vkDestroyAccelerationStructureKHR(device->getHandle(), handle, nullptr);
	}

	VkAccelerationStructureKHR VK_AABBBottomLevelAccelerationStructure::getHandle() const
	{
		return handle;
	}

	VkDeviceOrHostAddressConstKHR VK_AABBBottomLevelAccelerationStructure::getDeviceAddress() const
	{
		return address;
	}
}
