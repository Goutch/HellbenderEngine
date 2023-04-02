#include "VK_PipelineLayout.h"
#include "VK_Device.h"
#include "algorithm"
#include "VK_Image.h"
#include "core/resource/Resources.h"
#include "VK_RenderPass.h"

namespace HBE {

	VK_PipelineLayout::VK_PipelineLayout(VK_Device *device,
										 const VK_Shader **shaders,
										 size_t count) {

		this->device = device;
		std::vector<VK_UniformInfo> merged_unfiorms;
		std::vector<VK_PushConstantInfo> merged_push_constants;
		if (shaders[0]->getVkStage() == VK_SHADER_STAGE_COMPUTE_BIT) {
			bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
		}
		if (shaders[0]->getVkStage() == VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
			bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		}


		//merge the shaders inputs into one.
		for (size_t i = 0; i < count; ++i) {
			//merge uniforms
			std::vector<VK_UniformInfo> uniforms = shaders[i]->getUniforms();
			for (size_t j = 0; j < uniforms.size(); ++j) {
				auto it = uniform_binding_to_index.find(uniforms[j].layout_binding.binding);

				if (it != uniform_binding_to_index.end()) {
					uint32_t merged_index = it->second;
					HB_ASSERT(merged_unfiorms[it->second].name == uniforms[j].name, "Binding#" + std::to_string(uniforms[j].layout_binding.binding) + " has multiple name definitions");
					HB_ASSERT(merged_unfiorms[it->second].size == uniforms[j].size, "Binding#" + std::to_string(uniforms[j].layout_binding.binding) + " has different sizes");
					HB_ASSERT(merged_unfiorms[it->second].layout_binding.descriptorCount == uniforms[j].layout_binding.descriptorCount, "Binding#" + std::to_string(uniforms[j].layout_binding.binding) + " has different array counts");
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
			std::vector<VK_PushConstantInfo> push_constants = shaders[i]->getPushConstants();
			for (int j = 0; j < push_constants.size(); ++j) {
				merged_push_constants.emplace_back(push_constants[j]);
				push_constants_ranges.emplace_back(push_constants[j].push_constant_range);
				push_constant_name_to_index.emplace(push_constants[j].name, merged_push_constants.size() - 1);

			}
		}

		/*std::sort(uniform_descriptor_set_layout_bindings.begin(), uniform_descriptor_set_layout_bindings.end(),
				  [](VkDescriptorSetLayoutBinding i, VkDescriptorSetLayoutBinding j) {
					  return (i.binding < j.binding);
				  });*/
		std::vector<VkDescriptorBindingFlagsEXT> binding_flags(uniform_descriptor_set_layout_bindings.size(), 0);
		variable_descriptor_count.resize(uniform_descriptor_set_layout_bindings.size(), false);
		int variable_size_binding = -1;
		for (int i = 0; i < merged_unfiorms.size(); ++i) {
			if (merged_unfiorms[i].variable_size) {
				HB_ASSERT(variable_size_binding < 0, "Only one variable size binding per descriptor set is allowed");
				variable_descriptor_count[i] = true;
				variable_size_binding = merged_unfiorms[i].layout_binding.binding;
				binding_flags[i] = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
			}
		}
		if (variable_size_binding != -1) {
			uint32_t binding_index = uniform_binding_to_index[variable_size_binding];
			VK_UniformInfo &uniform_info = merged_unfiorms[binding_index];
			for (int i = 0; i < merged_unfiorms.size(); ++i) {
				HB_ASSERT(variable_size_binding >= merged_unfiorms[i].layout_binding.binding, "Variable size binding must be the last binding of the set");
				if (variable_size_binding != merged_unfiorms[i].layout_binding.binding &&
					merged_unfiorms[i].layout_binding.descriptorType == uniform_info.layout_binding.descriptorType) {
					uniform_info.layout_binding.descriptorCount -= merged_unfiorms[i].layout_binding.descriptorCount;
					uniform_descriptor_set_layout_bindings[binding_index].descriptorCount -= merged_unfiorms[i].layout_binding.descriptorCount;
				}
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uniform_descriptor_set_layout_bindings.size();
		layoutInfo.pBindings = uniform_descriptor_set_layout_bindings.data();

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsInfo{};
		if(variable_size_binding != -1)
		{
			layoutInfo.pNext = &flagsInfo;
			flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
			flagsInfo.bindingCount = uniform_descriptor_set_layout_bindings.size();
			flagsInfo.pBindingFlags = binding_flags.data();
		}

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

	VkDescriptorSetLayout VK_PipelineLayout::getDescriptorSetLayout() const {
		return descriptor_set_layout_handle;
	}

	bool VK_PipelineLayout::IsBindingVariableSize(uint32_t binding) const {
		return variable_descriptor_count[uniform_binding_to_index.find(binding)->second];
	}


}
