#pragma once

#include <array>
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_Shader.h"

namespace HBE {
	class VK_Shader;

	class VK_Device;

	class RenderTarget;

	class Texture;

	class VK_DescriptorSetLayout;

	class VK_PipelineLayout {
		VkPipelineLayout handle = VK_NULL_HANDLE;
		VK_Device *device = nullptr;
		std::vector<VkDescriptorSetLayout> descriptor_set_layout_handles;
		std::vector<VK_DescriptorSetLayout *> descriptor_set_layouts;
		std::vector<VkDeviceSize> descriptor_sizes;
		std::map<std::string, size_t> descriptor_name_to_binding;
		std::vector<bool> variable_descriptors;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::vector<VkPushConstantRange> push_constants_ranges;
		std::map<std::string, size_t> push_constant_name_to_index;
		std::vector<VK_DescriptorInfo> pipeline_descriptors;
		std::vector<VK_PushConstantInfo> pipeline_push_constants;

		VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
	public:
		VK_PipelineLayout(VK_Device *device, const VK_Shader **shaders, size_t count);

		~VK_PipelineLayout();

		VkPipelineLayout getHandle() const;

		void pushConstant(VkCommandBuffer command_buffer, const std::string &name, const void *data) const;

		const std::vector<VkDescriptorSetLayoutBinding> &getDescriptorBindings() const;

		uint32_t getDescriptorBinding(const std::string &name) const;

		VkPipelineBindPoint getBindPoint() const;

		const std::vector<VkDeviceSize> &getDescriptorSizes() const;

		const std::vector<VkDescriptorSetLayout> &getDescriptorSetLayoutHandles() const;

		const std::vector<VK_DescriptorInfo> &getDescriptorInfos() const;

		bool IsBindingVariableSize(uint32_t binding) const;

		void mergeStages(const VK_Shader **shaders, size_t count);

		void mergeDescriptorStages(VK_DescriptorInfo &merged_descriptor, VK_DescriptorInfo &old_descriptor, VK_DescriptorInfo &new_descriptor);

		uint32_t getLastDescriptorSetBinding(uint32_t set) const;

		uint32_t getDescriptorSetCount() const;
	};
}


