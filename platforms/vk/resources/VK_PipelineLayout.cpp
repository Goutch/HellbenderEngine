#include "platforms/vk/resources/VK_PipelineLayout.h"
#include "../VK_Device.h"
#include "algorithm"
#include "../VK_Context.h"
#include "VK_Image.h"
#include "VK_RenderPass.h"
#include "../VK_DescriptorSetLayout.h"
#include "core/utility/Log.h"

namespace HBE {
	void VK_PipelineLayout::init(VK_Context *context, ShaderHandle *shaders, size_t count, bool empty_descriptor_allowed) {
		this->context = context;

		VkShaderStageFlagBits shaderStage = context->shaders[shaders[0]].getVkStage();
		if (shaderStage == VK_SHADER_STAGE_COMPUTE_BIT) {
			bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
		}
		if (shaderStage == VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
			bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		}
		mergeStages(shaders, count);

		//map sets ids to an index
		for (int i = 0; i < pipeline_bindings.size(); ++i) {
			if(descriptor_set_id_to_index.find(pipeline_bindings[i].descriptor_set_id) == descriptor_set_id_to_index.end()) {
				descriptor_set_layouts.emplace_back();
				descriptor_set_id_to_index.emplace(pipeline_bindings[i].descriptor_set_id, descriptor_set_layouts.size() - 1);
			}
		}
		descriptor_set_layout_handles.resize(descriptor_set_layouts.size());

		//per set
		for (int i = 0; i < descriptor_set_layouts.size(); ++i) {
			descriptor_set_layouts[i].init(context, i, pipeline_bindings, empty_descriptor_allowed);
			descriptor_set_layout_handles[i] = descriptor_set_layouts[i].getHandle();
		}

		layout_binding_handles.resize(pipeline_bindings.size());
		//per bindings
		for (int i = 0; i < pipeline_bindings.size(); ++i) {
			layout_binding_handles[i] = pipeline_bindings[i].layout_binding;
			descriptor_name_to_binding.emplace(pipeline_bindings[i].name, pipeline_bindings[i].layout_binding.binding);
			pipeline_bindings[i].descriptor_set_index = getDescriptorSetIndex(pipeline_bindings[i].descriptor_set_id);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = descriptor_set_layout_handles.size();
		pipelineLayoutInfo.pSetLayouts = descriptor_set_layout_handles.data();
		pipelineLayoutInfo.pushConstantRangeCount = push_constants_ranges.size();
		pipelineLayoutInfo.pPushConstantRanges = push_constants_ranges.data();

		if (vkCreatePipelineLayout(context->device.getHandle(), &pipelineLayoutInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create pipeline layout!");
		}
	}

	void VK_PipelineLayout::release() {
		for (int i = 0; i < descriptor_set_layouts.size(); ++i) {
			descriptor_set_layouts[i].release();
		}
		vkDestroyPipelineLayout(context->device.getHandle(), handle, nullptr);
	}

	VK_PipelineLayout::VK_PipelineLayout(VK_PipelineLayout &&other) noexcept {
		handle = other.handle;
		this->context = other.context;
		this->bind_point = other.bind_point;
		this->descriptor_set_layout_handles = std::move(other.descriptor_set_layout_handles);
		this->descriptor_set_layouts = std::move(other.descriptor_set_layouts);
		this->descriptor_name_to_binding = std::move(other.descriptor_name_to_binding);
		this->pipeline_bindings = std::move(other.pipeline_bindings);
		this->push_constants_ranges = std::move(other.push_constants_ranges);
		this->pipeline_bindings = std::move(other.pipeline_bindings);
		this->pipeline_push_constants = std::move(other.pipeline_push_constants);
		this->push_constant_name_to_index = std::move(other.push_constant_name_to_index);
		this->descriptor_set_id_to_index = std::move(other.descriptor_set_id_to_index);

		other.context = nullptr;
		other.descriptor_set_layout_handles.clear();
		other.descriptor_set_layouts.clear();
		other.descriptor_set_id_to_index.clear();
		other.descriptor_name_to_binding.clear();
		other.pipeline_bindings.clear();
		other.push_constants_ranges.clear();
		other.pipeline_bindings.clear();
		other.pipeline_push_constants.clear();
		other.push_constant_name_to_index.clear();
		other.handle = VK_NULL_HANDLE;
	}


	void VK_PipelineLayout::mergeStages(ShaderHandle *shaders, size_t count) {
		uint32_t max_descriptor_binding = 0;
		uint32_t max_descriptor_set = 0;
		const VK_BindingInfo DEFAULT_DESCRIPTOR_INFO = {
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
			std::vector<VK_BindingInfo> stage_bindings = context->shaders[shaders[i]].getDescriptorInfos();
			for (size_t j = 0; j < stage_bindings.size(); ++j) {
				VK_BindingInfo stage_binding_info = stage_bindings[j];
				uint32_t binding = stage_binding_info.layout_binding.binding;
				if (max_descriptor_binding <= binding) {
					max_descriptor_binding = binding;
					pipeline_bindings.resize(max_descriptor_binding + 1, DEFAULT_DESCRIPTOR_INFO);
				}
				if (pipeline_bindings[binding].layout_binding.stageFlags == 0) {
					pipeline_bindings[binding] = stage_binding_info;
				} else {
					mergeBindingInfoStages(pipeline_bindings[binding], pipeline_bindings[binding], stage_binding_info);
				}
			}

			//merge push_constants
			std::vector<VK_PushConstantInfo> stage_push_constants = context->shaders[shaders[i]].getPushConstants();
			for (int j = 0; j < stage_push_constants.size(); ++j) {
				Log::message("merge push constant:" + stage_push_constants[j].name);
				if (push_constant_name_to_index.find(stage_push_constants[j].name) == push_constant_name_to_index.end()) {
					pipeline_push_constants.emplace_back(stage_push_constants[j]);
					push_constants_ranges.emplace_back(stage_push_constants[j].push_constant_range);
					push_constant_name_to_index.emplace(stage_push_constants[j].name, pipeline_push_constants.size() - 1);
					continue;
				}
				uint32_t index = push_constant_name_to_index[stage_push_constants[j].name];
				HB_ASSERT(pipeline_push_constants[index].push_constant_range.offset == stage_push_constants[j].push_constant_range.offset,
				          "Push constant \"" + stage_push_constants[j].name + "\" has different offset");
				HB_ASSERT(pipeline_push_constants[index].push_constant_range.size == stage_push_constants[j].push_constant_range.size,
				          "Push constant \"" + stage_push_constants[j].name + "\" has different size");
				push_constants_ranges[index].stageFlags |= stage_push_constants[j].push_constant_range.stageFlags;
				pipeline_push_constants[index].push_constant_range.stageFlags |= stage_push_constants[j].push_constant_range.stageFlags;
			}
		}
	}

	void VK_PipelineLayout::mergeBindingInfoStages(VK_BindingInfo &merged_binding_info, VK_BindingInfo &old_binding_info, VK_BindingInfo &new_binding_info) {
		HB_ASSERT(old_binding_info.name == "" || new_binding_info.name == "" || old_binding_info.name == new_binding_info.name,
		          "Uniforms have different names:" + old_binding_info.name + " and " + new_binding_info.name);
		HB_ASSERT(old_binding_info.size == new_binding_info.size,
		          "Uniform \"" + old_binding_info.name = "\" Binding#" + std::to_string(old_binding_info.layout_binding.binding) + " has different sizes");
		HB_ASSERT(old_binding_info.layout_binding.descriptorType == new_binding_info.layout_binding.descriptorType,
		          "Uniform \"" + old_binding_info.name = "\" Binding#" + std::to_string(old_binding_info.layout_binding.binding) + " has different types");
		// HB_ASSERT(old_binding_info.variable_size == new_binding_info.variable_size,
		//           "Uniform \"" + old_binding_info.name = "\" Binding#" + std::to_string(old_binding_info.variable_size) + " has different variable size value");


		merged_binding_info.name = new_binding_info.name;
		merged_binding_info.size = new_binding_info.size;
		merged_binding_info.layout_binding.binding = new_binding_info.layout_binding.binding;
		merged_binding_info.layout_binding.descriptorType = new_binding_info.layout_binding.descriptorType;
		merged_binding_info.layout_binding.stageFlags = old_binding_info.layout_binding.stageFlags | new_binding_info.layout_binding.stageFlags;
		merged_binding_info.layout_binding.descriptorCount = old_binding_info.layout_binding.descriptorCount;

		if (old_binding_info.variable_size == new_binding_info.variable_size) {
			merged_binding_info.layout_binding.descriptorCount = old_binding_info.layout_binding.descriptorCount;
			merged_binding_info.variable_size = old_binding_info.variable_size;
			merged_binding_info.layout_binding.stageFlags = old_binding_info.layout_binding.stageFlags | new_binding_info.layout_binding.stageFlags;
		} else if (old_binding_info.variable_size && !new_binding_info.variable_size) {
			merged_binding_info.layout_binding.descriptorCount = old_binding_info.layout_binding.descriptorCount;
			merged_binding_info.variable_size = true;
			merged_binding_info.layout_binding.stageFlags = old_binding_info.layout_binding.stageFlags;
		} else if (!old_binding_info.variable_size && new_binding_info.variable_size) {
			merged_binding_info.layout_binding.descriptorCount = new_binding_info.layout_binding.descriptorCount;
			merged_binding_info.variable_size = true;
			merged_binding_info.layout_binding.stageFlags = new_binding_info.layout_binding.stageFlags;
		}
	}


	VkPipelineLayout VK_PipelineLayout::getHandle() const {
		return handle;
	}

	void VK_PipelineLayout::pushConstant(VkCommandBuffer command_buffer, const char *name, const void *data) const {
		auto it = push_constant_name_to_index.find(name);
		HB_ASSERT(it != push_constant_name_to_index.end(), "No push constant is named:" + std::string(name));
		vkCmdPushConstants(command_buffer,
		                   handle,
		                   push_constants_ranges[it->second].stageFlags,
		                   push_constants_ranges[it->second].offset,
		                   push_constants_ranges[it->second].size,
		                   data);
	}

	const std::vector<VkDescriptorSetLayoutBinding> &VK_PipelineLayout::getDescriptorBindings() const {
		return layout_binding_handles;
	}

	VkPipelineBindPoint VK_PipelineLayout::getBindPoint() const {
		return bind_point;
	}

	const std::vector<VkDescriptorSetLayout> &VK_PipelineLayout::getDescriptorSetLayoutHandles() const {
		return descriptor_set_layout_handles;
	}

	bool VK_PipelineLayout::IsBindingVariableSize(uint32_t binding) const {
		return pipeline_bindings[binding].variable_size;
	}

	uint32_t VK_PipelineLayout::getDescriptorBinding(const char *name) const {
		HB_ASSERT(descriptor_name_to_binding.find(name) != descriptor_name_to_binding.end(), "No descriptor is named:" + std::string(name));
		return descriptor_name_to_binding.find(name)->second;
	}

	const std::vector<VK_BindingInfo> &VK_PipelineLayout::getBindingInfos() const {
		return pipeline_bindings;
	}

	uint32_t VK_PipelineLayout::getLastDescriptorSetBinding(uint32_t set) const {
		return descriptor_set_layouts[set].getLastBinding();
	}

	uint32_t VK_PipelineLayout::getDescriptorSetCount() const {
		return descriptor_set_layouts.size();
	}

	const std::vector<VK_DescriptorSetLayout> &VK_PipelineLayout::getDescriptorSetLayouts() const {
		return descriptor_set_layouts;
	}

	VkDeviceSize VK_PipelineLayout::getBindingElementSize(uint32_t binding) const {
		return pipeline_bindings[binding].size;
	}

	uint32_t VK_PipelineLayout::getDescriptorSetIndex(uint32_t set_id) const {
		return descriptor_set_id_to_index.at(set_id);
	}
}
