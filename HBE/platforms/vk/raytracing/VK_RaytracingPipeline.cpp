//
// Created by user on 8/30/2022.
//

#include "VK_RaytracingPipeline.h"
#include "vulkan/vulkan.h"
#include "../VK_Renderer.h"
#include "../VK_PipelineLayout.h"
#include "../VK_Device.h"
#include "../VK_Renderer.h"
#include "../VK_CommandPool.h"
#include "vector"

namespace HBE {
	VK_RaytracingPipeline::VK_RaytracingPipeline(HBE::VK_Renderer *renderer, const HBE::RaytracingPipelineInfo &info) {
		this->renderer = renderer;
		this->device = renderer->getDevice();

		VK_Shader *raygen_shader = dynamic_cast<VK_Shader *>(info.raygen_shader);
		VK_Shader **miss_shaders = reinterpret_cast<VK_Shader **>(info.miss_shaders);
		VK_Shader **hit_shaders = reinterpret_cast<VK_Shader **>(info.hit_shaders);

		std::vector<const VK_Shader *> shaders;
		shaders.reserve(1 + info.miss_shader_count + info.hit_shader_count);
		shaders.push_back(raygen_shader);
		for (int i = 0; i < info.miss_shader_count; ++i) {
			shaders.push_back(miss_shaders[i]);
		}
		for (int i = 0; i < info.hit_shader_count; ++i) {
			shaders.push_back(hit_shaders[i]);
		}


		/*
			Setup ray tracing shader groups
		*/

		VkPipelineShaderStageCreateInfo closestHitShaderStageInfo{};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		shaderStages.reserve(1 + info.miss_shader_count + info.hit_shader_count);

		// Ray generation group
		{
			VkPipelineShaderStageCreateInfo raygenShaderStageInfo{};
			raygenShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			raygenShaderStageInfo.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			raygenShaderStageInfo.module = raygen_shader->getHandle();
			raygenShaderStageInfo.pName = "main";
			shaderStages.push_back(raygenShaderStageInfo);

			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
			shaderGroup.generalShader = 0;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
			shader_groups.push_back(shaderGroup);
		}

		// Miss group
		for (int i = 0; i < info.miss_shader_count; ++i) {

			VkPipelineShaderStageCreateInfo missShaderStageInfo{};
			missShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			missShaderStageInfo.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
			missShaderStageInfo.module = miss_shaders[i]->getHandle();
			missShaderStageInfo.pName = "main";
			shaderStages.push_back(missShaderStageInfo);

			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
			shaderGroup.generalShader = shaderStages.size() - 1;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
			shader_groups.push_back(shaderGroup);
		}

		for (int i = 0; i < info.hit_shader_count; ++i) {
			VkPipelineShaderStageCreateInfo hitShaderStageInfo{};
			hitShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			hitShaderStageInfo.stage = hit_shaders[i]->getVkStage();
			hitShaderStageInfo.module = hit_shaders[i]->getHandle();
			hitShaderStageInfo.pName = "main";
			shaderStages.push_back(hitShaderStageInfo);
		}

		uint32_t offset = info.miss_shader_count + 1;//raygen shader + miss shaders
		for (int i = 0; i < info.shader_group_count; ++i) {
			int32_t closest_hit_index = info.shader_groups[i].closest_hit_shader_index;
			int32_t any_hit_index = info.shader_groups[i].any_hit_shader_index;
			int32_t intersection_index = info.shader_groups[i].intersection_shader_index;

			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = intersection_index >= 0 ?
							   VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR :
							   VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
			shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.closestHitShader = closest_hit_index + offset;
			shaderGroup.anyHitShader = any_hit_index > 0 ? any_hit_index + offset : VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = intersection_index > 0 ? intersection_index + offset : VK_SHADER_UNUSED_KHR;
			shader_groups.push_back(shaderGroup);
		}

		pipeline_layout = new VK_PipelineLayout(device, shaders.data(), shaders.size());

		VkRayTracingPipelineCreateInfoKHR rayTracingPipelineInfo{};
		rayTracingPipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		rayTracingPipelineInfo.stageCount = shaderStages.size();
		rayTracingPipelineInfo.pStages = shaderStages.data();
		rayTracingPipelineInfo.groupCount = static_cast<uint32_t>(shader_groups.size());
		rayTracingPipelineInfo.pGroups = shader_groups.data();
		rayTracingPipelineInfo.maxPipelineRayRecursionDepth = 1;
		rayTracingPipelineInfo.layout = pipeline_layout->getHandle();

		if (device->vkCreateRayTracingPipelinesKHR(device->getHandle(),
												   VK_NULL_HANDLE,
												   VK_NULL_HANDLE,
												   1,
												   &rayTracingPipelineInfo,
												   nullptr,
												   &handle) != VK_SUCCESS) {
			Log::error("Failed to create ray tracing pipeline");
		}

		createShaderBindingTable(info);
	}

	uint32_t alignedSize(uint32_t value, uint32_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	}

	void VK_RaytracingPipeline::createShaderBindingTable(const RaytracingPipelineInfo &info) {
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR raytraycing_properties = device->getPhysicalDevice().getRayTracingPipelineProperties();
		const uint32_t handleSize = raytraycing_properties.shaderGroupHandleSize;
		const uint32_t handleSizeAligned = alignedSize(raytraycing_properties.shaderGroupHandleSize, raytraycing_properties.shaderGroupHandleAlignment);
		const uint32_t groupCount = static_cast<uint32_t>(shader_groups.size());
		const uint32_t sbtSize = groupCount * handleSizeAligned;

		std::vector<uint8_t> shaderHandleStorage(sbtSize);
		if (device->vkGetRayTracingShaderGroupHandlesKHR(device->getHandle(), handle, 0, groupCount, sbtSize, shaderHandleStorage.data())) {
			Log::error("Failed to get ray tracing shader group handles");
		}

		const VkBufferUsageFlags buffer_usage_flags = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		uint32_t offset = 0;
		raygen_shader_binding_table_buffer = new VK_Buffer(device, shaderHandleStorage.data(),
														   handleSize,
														   buffer_usage_flags,
														   ALLOC_FLAG_NONE);
		offset += handleSizeAligned;
		miss_shader_binding_table_buffer = new VK_Buffer(device, shaderHandleStorage.data() + offset,
														 handleSize * info.miss_shader_count,
														 buffer_usage_flags,
														 ALLOC_FLAG_NONE);
		offset += handleSizeAligned * info.miss_shader_count;
		hit_shader_binding_table_buffer = new VK_Buffer(device, shaderHandleStorage.data() + offset,
														handleSize * info.shader_group_count,
														buffer_usage_flags,
														ALLOC_FLAG_NONE);

		raygen_shader_binding_table_address.deviceAddress = raygen_shader_binding_table_buffer->getDeviceAddress().deviceAddress;
		raygen_shader_binding_table_address.stride = handleSizeAligned;
		raygen_shader_binding_table_address.size = handleSizeAligned;

		miss_shader_binding_table_address.deviceAddress = miss_shader_binding_table_buffer->getDeviceAddress().deviceAddress;
		miss_shader_binding_table_address.stride = handleSizeAligned;
		miss_shader_binding_table_address.size = handleSizeAligned * info.miss_shader_count;

		hit_shader_binding_table_address.deviceAddress = hit_shader_binding_table_buffer->getDeviceAddress().deviceAddress;
		hit_shader_binding_table_address.stride = handleSizeAligned;
		hit_shader_binding_table_address.size = handleSizeAligned * info.shader_group_count;

		VkStridedDeviceAddressRegionKHR callableShaderSbtEntry{};
	}

	void VK_RaytracingPipeline::bind() const {

		if (is_bound) return;
		is_bound = true;

		VkCommandBuffer command_buffer = renderer->getCommandPool()->getCurrentBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, handle);
	}

	void VK_RaytracingPipeline::unbind() const {
		is_bound = false;
	}

	const VkStridedDeviceAddressRegionKHR &VK_RaytracingPipeline::getRaygenShaderBindingTable() const {
		return raygen_shader_binding_table_address;
	}

	const VkStridedDeviceAddressRegionKHR &VK_RaytracingPipeline::getMissShaderBindingTable() const {
		return miss_shader_binding_table_address;
	}

	const VkStridedDeviceAddressRegionKHR &VK_RaytracingPipeline::getHitShaderBindingTable() const {
		return hit_shader_binding_table_address;
	}

	const VkStridedDeviceAddressRegionKHR &VK_RaytracingPipeline::getCallableShaderBindingTable() const {
		return callable_shader_binding_table_address;
	}

	const VK_PipelineLayout *VK_RaytracingPipeline::getPipelineLayout() const {
		return pipeline_layout;
	}

	VK_RaytracingPipeline::~VK_RaytracingPipeline() {
		delete pipeline_layout;
		delete shader_binding_table_buffer;
		delete raygen_shader_binding_table_buffer;
		delete miss_shader_binding_table_buffer;
		delete hit_shader_binding_table_buffer;
		vkDestroyPipeline(device->getHandle(), handle, nullptr);
	}

}
