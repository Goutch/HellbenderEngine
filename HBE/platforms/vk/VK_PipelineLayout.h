#pragma once

#include <array>
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_CONSTANTS.h"
#include "VK_Shader.h"

namespace HBE {
	class VK_Shader;

	class VK_Device;

	class RenderTarget;

	class Texture;

	class VK_PipelineLayout {
		VkPipelineLayout handle = VK_NULL_HANDLE;
		VK_Device *device = nullptr;
		VkDescriptorSetLayout descriptor_set_layout_handle = VK_NULL_HANDLE;

		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_set_handles;
		std::vector<VkDescriptorSetLayoutBinding> uniform_descriptor_set_layout_bindings;
		std::vector<VkDeviceSize> uniform_sizes;
		std::map<std::string, size_t> uniform_name_to_index;
		std::map<uint32_t, size_t> uniform_binding_to_index;

		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> uniform_buffers;

		std::vector<VkPushConstantRange> push_constants_ranges;
		std::map<std::string, size_t> push_constant_name_to_index;


		VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;


	public:
		VK_PipelineLayout(VK_Device *device, const VK_Shader **shaders, size_t count);
		~VK_PipelineLayout();

		VkPipelineLayout getHandle() const;

		void pushConstant(VkCommandBuffer command_buffer, const std::string &name, const void *data) const;

		const std::vector<VkDescriptorSetLayoutBinding> &getDescriptorSetLayoutBindings() const;
		const std::map<std::string, size_t> &getUniformNameToIndex() const;
		const std::map<uint32_t, size_t> &getUniformBindingToIndex() const;
		VkPipelineBindPoint getBindPoint() const;
		const std::vector<VkDeviceSize> &getUniformSizes() const;

		VkDescriptorSetLayout getDescriptorSetLayout() const;
	};
}


