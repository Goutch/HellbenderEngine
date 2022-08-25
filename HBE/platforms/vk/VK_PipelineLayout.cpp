#include "VK_PipelineLayout.h"
#include "VK_Device.h"
#include "algorithm"
#include "VK_Image.h"
#include "core/resource/Resources.h"
#include "VK_RenderPass.h"

namespace HBE {

	VK_PipelineLayout::VK_PipelineLayout(VK_Device *device, const VK_Shader **shaders,
										 size_t count) {

		this->device = device;
		std::vector<UniformInfo> merged_unfiorms;
		std::vector<PushConstantInfo> merged_push_constants;
		if (shaders[0]->getVkStage() == VK_SHADER_STAGE_COMPUTE_BIT) {
			bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
		}
		//merge the shaders inputs into one.
		for (size_t i = 0; i < count; ++i) {
			//merge uniforms
			std::vector<UniformInfo> uniforms = shaders[i]->getUniforms();
			for (size_t j = 0; j < uniforms.size(); ++j) {
				auto it = uniform_binding_to_index.find(uniforms[j].layout_binding.binding);

				if (it != uniform_binding_to_index.end()) {
					uint32_t merged_index = it->second;
					HB_ASSERT(merged_unfiorms[it->second].name == uniforms[j].name, "Binding#" + std::to_string(uniforms[j].layout_binding.binding) + " has multiple name definitions");
					HB_ASSERT(merged_unfiorms[it->second].size == uniforms[j].size, "Binding#" + std::to_string(uniforms[j].layout_binding.binding) + " has different sizes");
					//merge shader stages.
					merged_unfiorms[merged_index].layout_binding.stageFlags |= uniforms[j].layout_binding.stageFlags;
					uniform_descriptor_set_layout_bindings[merged_index].stageFlags |= uniforms[j].layout_binding.stageFlags;
				} else {
					merged_unfiorms.emplace_back(uniforms[j]);
					uniform_sizes.emplace_back(uniforms[j].size);
					uniform_descriptor_set_layout_bindings.emplace_back(uniforms[j].layout_binding);

					uniform_binding_to_index.emplace(uniforms[j].layout_binding.binding, merged_unfiorms.size() - 1);
					uniform_name_to_index.emplace(uniforms[j].name, merged_unfiorms.size() - 1);
				}
			}

			//merge push_constants
			std::vector<PushConstantInfo> push_constants = shaders[i]->getPushConstants();
			for (int j = 0; j < push_constants.size(); ++j) {
				merged_push_constants.emplace_back(push_constants[j]);
				push_constants_ranges.emplace_back(push_constants[j].push_constant_range);
				push_constant_name_to_index.emplace(push_constants[j].name, merged_push_constants.size() - 1);

			}
		}
		std::sort(uniform_descriptor_set_layout_bindings.begin(), uniform_descriptor_set_layout_bindings.end(),
				  [](VkDescriptorSetLayoutBinding i, VkDescriptorSetLayoutBinding j) {
					  return (i.binding < j.binding);
				  });

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uniform_descriptor_set_layout_bindings.size();
		layoutInfo.pBindings = uniform_descriptor_set_layout_bindings.data();

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &descriptor_set_layout_handle);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout_handle;
		pipelineLayoutInfo.pushConstantRangeCount = push_constants_ranges.size();
		pipelineLayoutInfo.pPushConstantRanges = push_constants_ranges.data();

		if (vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create pipeline layout!");
		}
	}

	VkPipelineLayout VK_PipelineLayout::getHandle() const {
		return handle;
	}

	VK_PipelineLayout::~VK_PipelineLayout() {
		if (descriptor_set_layout_handle != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(device->getHandle(), descriptor_set_layout_handle, nullptr);
		vkDestroyPipelineLayout(device->getHandle(), handle, nullptr);
	}

	void VK_PipelineLayout::pushConstant(VkCommandBuffer command_buffer, const std::string &name, const void *data) const {
		auto it = push_constant_name_to_index.find(name);
		HB_ASSERT(it != push_constant_name_to_index.end(), "No push constant is named:" + name);
		vkCmdPushConstants(command_buffer,
						   handle,
						   push_constants_ranges[it->second].stageFlags,
						   push_constants_ranges[it->second].offset,
						   push_constants_ranges[it->second].size,
						   data);
	}

	const std::vector<VkDescriptorSetLayoutBinding> &VK_PipelineLayout::getDescriptorSetLayoutBindings() const {
		return uniform_descriptor_set_layout_bindings;
	}

	const std::map<std::string, size_t> &VK_PipelineLayout::getUniformNameToIndex() const {
		return uniform_name_to_index;
	}

	const std::map<uint32_t, size_t> &VK_PipelineLayout::getUniformBindingToIndex() const {
		return uniform_binding_to_index;
	}

	VkPipelineBindPoint VK_PipelineLayout::getBindPoint() const {
		return bind_point;
	}

	const std::vector<VkDeviceSize> &VK_PipelineLayout::getUniformSizes() const {
		return uniform_sizes;
	}

	VkDescriptorSetLayout VK_PipelineLayout::getDescriptorSetLayout() const{
		return descriptor_set_layout_handle;
	}


}
