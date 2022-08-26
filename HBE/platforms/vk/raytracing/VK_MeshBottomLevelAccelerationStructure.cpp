#include "VK_MeshBottomLevelAccelerationStructure.h"
#include "../VK_Device.h"
#include "../VK_Buffer.h"
#include "../VK_Mesh.h"

namespace HBE {
	VK_MeshBottomLevelAccelerationStructure::VK_MeshBottomLevelAccelerationStructure(VK_Device *device, MeshAccelerationStructureInfo info) {


		this->device = device;


		VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress{};

		//todo:initialize this
		VK_Mesh *mesh = dynamic_cast<VK_Mesh *>(info.mesh);
		VkDeviceSize vertex_size = mesh->getBindingSize(0);

		VkBufferDeviceAddressInfo buffer_address_info{};
		buffer_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

		buffer_address_info.buffer = mesh->getBuffer(0)->getHandle();
		vertexBufferDeviceAddress.deviceAddress = vkGetBufferDeviceAddress(device->getHandle(), &buffer_address_info);

		buffer_address_info.buffer = mesh->getIndicesBuffer()->getHandle();
		indexBufferDeviceAddress.deviceAddress = vkGetBufferDeviceAddress(device->getHandle(), &buffer_address_info);

		buffer_address_info.buffer = mesh->getBuffer(1)->getHandle();
		transformBufferDeviceAddress.deviceAddress = vkGetBufferDeviceAddress(device->getHandle(), &buffer_address_info);

		VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;

		accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		accelerationStructureGeometry.geometry.triangles.vertexData = vertexBufferDeviceAddress;
		accelerationStructureGeometry.geometry.triangles.maxVertex = mesh->getVertexCount();
		accelerationStructureGeometry.geometry.triangles.vertexStride = vertex_size;
		accelerationStructureGeometry.geometry.triangles.indexType = mesh->getIndicesSize() == sizeof(uint32_t) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
		accelerationStructureGeometry.geometry.triangles.indexData = indexBufferDeviceAddress;
		accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
		accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;
		accelerationStructureGeometry.geometry.triangles.transformData = transformBufferDeviceAddress;


		// Get size info
		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationStructureBuildGeometryInfo.geometryCount = 1;
		accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

		const uint32_t numTriangles = mesh->hasIndexBuffer() ? mesh->getIndexCount() / 3 : mesh->getVertexCount() / 3;
		VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
		accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		device->vkGetAccelerationStructureBuildSizesKHR(
				device->getHandle(),
				VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
				&accelerationStructureBuildGeometryInfo,
				&numTriangles,
				&accelerationStructureBuildSizesInfo);

		buffer = new VK_Buffer(device,
							   accelerationStructureBuildSizesInfo.accelerationStructureSize,
							   VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
							   ALLOC_FLAGS::ALLOC_FLAG_NONE);


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
											ALLOC_FLAG_NONE);

		buffer_address_info.buffer = scratchBuffer.getHandle();
		VkDeviceOrHostAddressConstKHR scratch_buffer_address{};
		scratch_buffer_address.deviceAddress = device->vkGetBufferDeviceAddressKHR(device->getHandle(), &buffer_address_info);

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = handle;
		accelerationBuildGeometryInfo.geometryCount = 1;
		accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
		accelerationBuildGeometryInfo.scratchData.deviceAddress = scratch_buffer_address.deviceAddress;

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = numTriangles;
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
		device->getQueue(QUEUE_FAMILY_GRAPHICS).submitCommand();

		VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
		accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		accelerationDeviceAddressInfo.accelerationStructure = handle;

		VkDeviceOrHostAddressConstKHR address{};
		address.deviceAddress = device->vkGetAccelerationStructureDeviceAddressKHR(device->getHandle(), &accelerationDeviceAddressInfo);
	}

	VK_MeshBottomLevelAccelerationStructure::~VK_MeshBottomLevelAccelerationStructure() {
		delete buffer;
		device->vkDestroyAccelerationStructureKHR(device->getHandle(), handle, nullptr);
	}
}