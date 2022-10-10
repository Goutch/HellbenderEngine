#include <platforms/vk/VK_Buffer.h>
#include <core/utility/Profiler.h>
#include "VK_TopLevelAccelerationStructure.h"
#include "vector"
#include "VK_AABBBottomLevelAccelerationStructure.h"
#include "VK_MeshBottomLevelAccelerationStructure.h"
#include "../VK_Device.h"
#include "../VK_CommandPool.h"

namespace HBE {


	VK_TopLevelAccelerationStructure::VK_TopLevelAccelerationStructure(VK_Device *device, const RootAccelerationStructureInfo &info) {
		this->device = device;
		Profiler::begin("Build root Acceleration Structure");
		std::vector<VkAccelerationStructureInstanceKHR> instances;
		instances.resize(info.instance_count);
		for (int i = 0; i < info.instance_count; ++i) {

			VkTransformMatrixKHR t = {
					{
							{info.instances[i].transform[0][0], info.instances[i].transform[1][0], info.instances[i].transform[2][0], info.instances[i].transform[3][0]},
							{info.instances[i].transform[0][1], info.instances[i].transform[1][1], info.instances[i].transform[2][1], info.instances[i].transform[3][1]},
							{info.instances[i].transform[0][2], info.instances[i].transform[1][2], info.instances[i].transform[2][2], info.instances[i].transform[3][2]},
					}
			};

			instances[i].transform = t;
			instances[i].instanceCustomIndex = info.instances[i].custom_index;
			instances[i].mask = 0xFF;
			instances[i].instanceShaderBindingTableRecordOffset = info.instances[i].shader_group_index;
			instances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
			if (info.instances[i].type == ACCELERATION_STRUCTURE_TYPE_AABB) {
				auto *aabb_as = (VK_AABBBottomLevelAccelerationStructure *) info.aabb_acceleration_structures[info.instances[i].acceleration_structure_index];
				instances[i].accelerationStructureReference = aabb_as->getDeviceAddress().deviceAddress;
			} else {
				auto mesh_as = (VK_MeshBottomLevelAccelerationStructure *) info.mesh_acceleration_structures[info.instances[i].acceleration_structure_index];
				instances[i].accelerationStructureReference = mesh_as->getDeviceAddress().deviceAddress;
			}
		}

		VK_Buffer instances_buffer(device, instances.data(),
								   sizeof(VkAccelerationStructureInstanceKHR) * instances.size(),
								   VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);


		VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		accelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		accelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
		accelerationStructureGeometry.geometry.instances.data = instances_buffer.getDeviceAddress();

		// Get size info
		/*
		The pSrcAccelerationStructure, dstAccelerationStructure, and mode members of pBuildInfo are ignored. Any VkDeviceOrHostAddressKHR members of pBuildInfo are ignored by this command, except that the hostAddress member of VkAccelerationStructureGeometryTrianglesDataKHR::transformData will be examined to check if it is NULL.*
		*/
		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationStructureBuildGeometryInfo.geometryCount = 1;
		accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

		uint32_t primitive_count = instances.size();

		VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
		accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		device->vkGetAccelerationStructureBuildSizesKHR(
				device->getHandle(),
				VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
				&accelerationStructureBuildGeometryInfo,
				&primitive_count,
				&accelerationStructureBuildSizesInfo);


		buffer = new VK_Buffer(device,
							   accelerationStructureBuildSizesInfo.accelerationStructureSize,
							   VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.buffer = buffer->getHandle();
		accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
		accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		device->vkCreateAccelerationStructureKHR(device->getHandle(), &accelerationStructureCreateInfo, nullptr, &handle);

		// Create a small scratch buffer used during build of the top level acceleration structure
		VK_Buffer scratchBuffer = VK_Buffer(device,
											accelerationStructureBuildSizesInfo.buildScratchSize,
											VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
											ALLOC_FLAG_NONE);

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = handle;
		accelerationBuildGeometryInfo.geometryCount = 1;
		accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
		accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress().deviceAddress;

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = instances.size();
		accelerationStructureBuildRangeInfo.primitiveOffset = 0;
		accelerationStructureBuildRangeInfo.firstVertex = 0;
		accelerationStructureBuildRangeInfo.transformOffset = 0;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR *> accelerationBuildStructureRangeInfos = {&accelerationStructureBuildRangeInfo};

		// Build the acceleration structure on the device via a one-time command buffer submission
		// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
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
		Profiler::end();
	}

	VkDeviceOrHostAddressConstKHR VK_TopLevelAccelerationStructure::getDeviceAddress() const {
		return address;
	}

	VK_TopLevelAccelerationStructure::~VK_TopLevelAccelerationStructure() {
		delete buffer;
		device->vkDestroyAccelerationStructureKHR(device->getHandle(), handle, nullptr);

	}

	const VkAccelerationStructureKHR VK_TopLevelAccelerationStructure::getHandle() const {
		return handle;
	}

}