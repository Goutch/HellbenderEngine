#include "VK_PipelineLayout.h"
#include "VK_Device.h"
#include "algorithm"
#include "VK_Image.h"
#include "core/resource/Resources.h"
#include "VK_RenderPass.h"

namespace HBE {

	void mergeDescriptorStages(VK_DescriptorInfo &merged_descriptor, VK_DescriptorInfo &old_descriptor, VK_DescriptorInfo &new_descriptor) {
		HB_ASSERT(old_descriptor.name == new_descriptor.name, "Uniforms have different names:" + old_descriptor.name + " and " + new_descriptor.name);
		HB_ASSERT(old_descriptor.size == new_descriptor.size, "Uniform \"" + old_descriptor.name = "\" Binding#" + std::to_string(old_descriptor.layout_binding.binding) + " has different sizes");
		HB_ASSERT(old_descriptor.layout_binding.descriptorType == new_descriptor.layout_binding.descriptorType,
				  "Uniform \"" + old_descriptor.name = "\" Binding#" + std::to_string(old_descriptor.layout_binding.binding) + " has different types");

		merged_descriptor.name = new_descriptor.name;
		merged_descriptor.size = new_descriptor.size;
		merged_descriptor.layout_binding.binding = new_descriptor.layout_binding.binding;
		merged_descriptor.layout_binding.descriptorType = new_descriptor.layout_binding.descriptorType;
		merged_descriptor.layout_binding.stageFlags = old_descriptor.layout_binding.stageFlags | new_descriptor.layout_binding.stageFlags;


		if (old_descriptor.variable_size == new_descriptor.variable_size) {
			merged_descriptor.layout_binding.descriptorCount = old_descriptor.layout_binding.descriptorCount;
			merged_descriptor.variable_size = old_descriptor.variable_size;
		} else if (old_descriptor.variable_size && !new_descriptor.variable_size) {
			merged_descriptor.layout_binding.descriptorCount = old_descriptor.layout_binding.descriptorCount;
			merged_descriptor.variable_size = true;
		} else if (!old_descriptor.variable_size && new_descriptor.variable_size) {
			merged_descriptor.layout_binding.descriptorCount = new_descriptor.layout_binding.descriptorCount;
			merged_descriptor.variable_size = true;
		}
	}

	void VK_PipelineLayout::mergeStages(VK_Shader **shaders, size_t count) {
		uint32_t max_descriptor_binding = 0;

		const VK_DescriptorInfo DEFAULT_DESCRIPTOR_INFO = {
				.name = "",
				.size = 0,
				.layout_binding = {
						.binding = 0,
						.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM,
						.descriptorCount = 1,
						.stageFlags = 0,
						.pImmutableSamplers = nullptr
				},
				.variable_size = false,
		};

		//merge all the stages descriptors into pipeline_descriptors
		for (size_t i = 0; i < count; ++i) {
			std::vector<VK_DescriptorInfo> stage_descriptors = shaders[i]->getDescriptorInfos();
			for (size_t j = 0; j < stage_descriptors.size(); ++j) {
				VK_DescriptorInfo stage_descriptor = stage_descriptors[j];
				uint32_t descriptor_binding = stage_descriptor.layout_binding.binding;

				if (max_descriptor_binding < descriptor_binding) {
					max_descriptor_binding = descriptor_binding;
					pipeline_descriptors.resize(max_descriptor_binding + 1, DEFAULT_DESCRIPTOR_INFO);
				}
				if (pipeline_descriptors[descriptor_binding].layout_binding.stageFlags == 0) {
					pipeline_descriptors[descriptor_binding] = stage_descriptor;
				} else {
					mergeDescriptorStages(pipeline_descriptors[descriptor_binding], pipeline_descriptors[descriptor_binding], stage_descriptor);
				}
			}

			//merge push_constants
			std::vector<VK_PushConstantInfo> stage_push_constants = shaders[i]->getPushConstants();
			for (int j = 0; j < stage_push_constants.size(); ++j) {
				pipeline_push_constants.emplace_back(stage_push_constants[j]);
				push_constants_ranges.emplace_back(stage_push_constants[j].push_constant_range);
				push_constant_name_to_index.emplace(stage_push_constants[j].name, pipeline_push_constants.size() - 1);
			}
		}
	}

	VK_PipelineLayout::VK_PipelineLayout(VK_Device *device,
										 const VK_Shader **shaders,
										 size_t count) {

		this->device = device;

		if (shaders[0]->getVkStage() == VK_SHADER_STAGE_COMPUTE_BIT) {
			bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
		}
		if (shaders[0]->getVkStage() == VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
			bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		}


		bool has_a_variable_size_binding = false;
		layout_bindings.resize(pipeline_descriptors.size());
		variable_descriptors.resize(pipeline_descriptors.size(), false);
		std::vector<VkDescriptorBindingFlagsEXT> descriptor_binding_flags(pipeline_descriptors.size(), 0);
		for (int i = 0; i < pipeline_descriptors.size(); ++i) {
			if (pipeline_descriptors[i].variable_size) {
				has_a_variable_size_binding = true;
				for (int j = 0; j < pipeline_descriptors.size(); ++j) {
					if (j != i && pipeline_descriptors[i].set == pipeline_descriptors[j].set) {
						pipeline_descriptors[i].layout_binding.descriptorCount -= pipeline_descriptors[j].layout_binding.descriptorCount;
					}
				}
			}
			layout_bindings[i] = pipeline_descriptors[i].layout_binding;
			variable_descriptors[i] = pipeline_descriptors[i].variable_size;
			descriptor_binding_flags[i] = pipeline_descriptors[i].variable_size ? VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT : 0;
		}


		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = descriptor_set_layout_bindings.size();
		layoutInfo.pBindings = descriptor_set_layout_bindings.data();

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsInfo{};
		if (has_a_variable_size_binding) {
			layoutInfo.pNext = &flagsInfo;
			flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
			flagsInfo.bindingCount = descriptor_set_layout_bindings.size();
			flagsInfo.pBindingFlags = descriptor_binding_flags.data();
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
		return descriptor_set_layout_bindings;
	}

	VkPipelineBindPoint VK_PipelineLayout::getBindPoint() const {
		return bind_point;
	}

	const std::vector<VkDeviceSize> &VK_PipelineLayout::getDescriptorSizes() const {
		return uniform_sizes;
	}

	VkDescriptorSetLayout VK_PipelineLayout::getDescriptorSetLayout() const {
		return descriptor_set_layout_handle;
	}

	bool VK_PipelineLayout::IsBindingVariableSize(uint32_t binding) const {
		return variable_descriptors[binding];
	}

	uint32_t VK_PipelineLayout::getDescriptorBinding(const std::string &name) const {
		return descriptor_name_to_binding.find(name)->second;
	}


}
