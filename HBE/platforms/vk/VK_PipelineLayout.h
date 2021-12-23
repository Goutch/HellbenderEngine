#pragma once

#include <array>
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_CONSTANTS.h"
#include "VK_Shader.h"
namespace HBE {
	class VK_Shader;

	class VK_Device;

	class VK_PipelineLayout {
		VkPipelineLayout handle = VK_NULL_HANDLE;
		VK_Device* device= nullptr;
		VkDescriptorSetLayout descriptor_set_layout_handle = VK_NULL_HANDLE;
		VkDescriptorPool descriptor_pool_handle = VK_NULL_HANDLE;
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_set_handles;
		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;
		std::vector<VkPushConstantRange> push_constants_ranges;
		std::array<std::unordered_map<uint32_t ,VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptor_sets_writes;
		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> uniform_buffers;
		std::vector<UniformInput> inputs;
		std::unordered_map<std::string,uint32_t> name_input_index;
		std::unordered_map<uint32_t ,uint32_t> binding_input_index;
		std::unordered_map<std::string,uint32_t> name_push_constant_index;
		VK_PipelineLayout(VK_Device *device, std::vector<VK_Shader *> shaders);
		~VK_PipelineLayout();
		void createDescriptorSets();
	};
}


