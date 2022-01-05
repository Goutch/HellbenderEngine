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
		for (size_t i = 0; i < count; ++i) {
			auto shader_bindings = shaders[i]->getLayoutBindings();
			descriptor_set_layout_bindings.insert(descriptor_set_layout_bindings.end(), shader_bindings.begin(),
												  shader_bindings.end());

			auto shader_ranges = shaders[i]->getPushConstantRanges();
			push_constants_ranges.insert(push_constants_ranges.end(), shader_ranges.begin(), shader_ranges.end());

			auto shader_inputs = shaders[i]->getInputs();

			for (size_t j = 0; j < shader_inputs.size(); ++j) {
				name_input_index.emplace(shader_inputs[j].name, inputs.size() + j);
				binding_input_index.emplace(shader_inputs[j].binding, inputs.size() + j);
			}
			inputs.insert(inputs.end(), shader_inputs.begin(), shader_inputs.end());
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = descriptor_set_layout_bindings.size();
		layoutInfo.pBindings = descriptor_set_layout_bindings.data();

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &descriptor_set_layout_handle);

		for (size_t i = 0; i < descriptor_set_layout_bindings.size(); ++i) {
			if (descriptor_set_layout_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j) {
					uniform_buffers.emplace(descriptor_set_layout_bindings[i].binding, std::vector<VK_Buffer *>());
					auto it = binding_input_index.find(descriptor_set_layout_bindings[i].binding);
					if (it != binding_input_index.end()) {
						uniform_buffers[descriptor_set_layout_bindings[i].binding].emplace_back(
								new VK_Buffer(device, inputs[it->second].size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
											  ALLOC_FLAGS::ALLOC_FLAG_MAPPABLE));
					} else {
						Log::error("Could not find input at binding " +
								   std::to_string(descriptor_set_layout_bindings[i].binding));
					}
				}
		}


		size_t combined_image_sampler_count = 0;
		size_t separate_image_count = 0;
		size_t storage_image_count = 0;
		size_t uniform_buffer_count = 0;
		for (auto layout_binding: descriptor_set_layout_bindings) {
			switch (layout_binding.descriptorType) {
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					uniform_buffer_count++;
					break;
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					combined_image_sampler_count++;
					break;
				case VK_DESCRIPTOR_TYPE_SAMPLER:
					break;
				case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					separate_image_count++;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					storage_image_count++;
					break;
				case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					break;
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					break;
				case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					break;
				case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:
					break;
				case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					break;
				case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
					break;
				case VK_DESCRIPTOR_TYPE_MAX_ENUM:
					break;
			}
		}
		std::vector<VkDescriptorPoolSize> poolSizes{};
		if (uniform_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = uniform_buffer_count * MAX_FRAMES_IN_FLIGHT;
		}
		if (combined_image_sampler_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[poolSizes.size() - 1].descriptorCount = combined_image_sampler_count * MAX_FRAMES_IN_FLIGHT;
		}
		if (storage_image_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_image_count * MAX_FRAMES_IN_FLIGHT;
		}
		if (separate_image_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			poolSizes[poolSizes.size() - 1].descriptorCount = separate_image_count * MAX_FRAMES_IN_FLIGHT;
		}


		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &descriptor_pool_handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
		createDescriptorSets();


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout_handle;
		pipelineLayoutInfo.pushConstantRangeCount = push_constants_ranges.size();
		pipelineLayoutInfo.pPushConstantRanges = push_constants_ranges.data();

		if (vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &handle) !=
			VK_SUCCESS) {
			Log::error("failed to create pipeline layout!");
		}
	}

	void VK_PipelineLayout::createDescriptorSets() {

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout_handle);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptor_pool_handle;
		allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();


		std::sort(descriptor_set_layout_bindings.begin(), descriptor_set_layout_bindings.end(),
				  [](VkDescriptorSetLayoutBinding i, VkDescriptorSetLayoutBinding j) {
					  return (i.binding < j.binding);
				  });
		if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, descriptor_set_handles.data()) != VK_SUCCESS) {
			Log::error("failed to allocate descriptor sets!");
		}
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			std::vector<VkWriteDescriptorSet> writes;
			std::vector<VkDescriptorBufferInfo> buffers_info;
			std::vector<VkDescriptorImageInfo> images_info;
			VK_Image *default_texture = (VK_Image *) Resources::get<Texture>("DEFAULT_TEXTURE");
			VK_Image *default_image = (VK_Image *) Resources::get<Texture>("DEFAULT_IMAGE");
			for (size_t j = 0; j < descriptor_set_layout_bindings.size(); ++j) {
				writes.emplace_back();
				buffers_info.emplace_back();
				images_info.emplace_back();
				writes[j].pBufferInfo = nullptr;
				writes[j].pImageInfo = nullptr;
				if (descriptor_set_layout_bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					buffers_info[j].buffer = uniform_buffers[j][i]->getHandle();
					buffers_info[j].offset = 0;
					auto it = binding_input_index.find(descriptor_set_layout_bindings[j].binding);
					if (it != binding_input_index.end()) {
						buffers_info[j].range = (VkDeviceSize) inputs[it->second].size;
						writes[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						writes[j].pBufferInfo = &buffers_info[j];
					} else {
						Log::error("Could not find input at binding " +
								   std::to_string(descriptor_set_layout_bindings[j].binding));
					}

				} else if (descriptor_set_layout_bindings[j].descriptorType ==
						   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					writes[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					images_info[j].imageView = default_texture->getImageView();
					images_info[j].sampler = default_texture->getSampler();
					images_info[j].imageLayout = default_texture->getImageLayout();
					writes[j].pImageInfo = &images_info[j];
				} else if (descriptor_set_layout_bindings[j].descriptorType ==
						   VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
					writes[j].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					images_info[j].imageView = default_image->getImageView();
					images_info[j].sampler = VK_NULL_HANDLE;
					images_info[j].imageLayout = default_image->getImageLayout();
					writes[j].pImageInfo = &images_info[j];
				} else if (descriptor_set_layout_bindings[j].descriptorType ==
						   VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
					writes[j].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					images_info[j].imageView = default_texture->getImageView();
					images_info[j].sampler = default_texture->getSampler();
					images_info[j].imageLayout = default_texture->getImageLayout();
					writes[j].pImageInfo = &images_info[j];
				} else {
					Log::error("Descriptor set type not supported yet");
				}

				writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writes[j].dstSet = descriptor_set_handles[i];
				writes[j].dstBinding = descriptor_set_layout_bindings[j].binding;
				writes[j].dstArrayElement = 0;

				writes[j].descriptorCount = 1;//for arrays
				writes[j].pTexelBufferView = nullptr; // Optional
				descriptor_sets_writes[i].emplace(writes[j].dstBinding, writes[j]);
				vkUpdateDescriptorSets(device->getHandle(), 1, &writes[j], 0, nullptr);
			}
		}
	}

	VkPipelineLayout VK_PipelineLayout::getHandle() {
		return handle;
	}

	VK_PipelineLayout::~VK_PipelineLayout() {
		for (auto buffer_vector: uniform_buffers) {
			for (size_t i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		if (descriptor_set_layout_handle != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(device->getHandle(), descriptor_set_layout_handle, nullptr);
		if (descriptor_pool_handle != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(device->getHandle(), descriptor_pool_handle, nullptr);
		vkDestroyPipelineLayout(device->getHandle(), handle, nullptr);
	}

	void VK_PipelineLayout::setTexture(uint32_t binding, const RenderTarget *render_target) {
		VK_RenderPass *render_pass = (VK_RenderPass *) render_target;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			const VK_Image *vk_image = render_pass->getImage(i);
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_image->getImageView();
			image_info.sampler = vk_image->getSampler();
			image_info.imageLayout = vk_image->getImageLayout();
			descriptor_sets_writes[i][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}
	}

	void VK_PipelineLayout::setTexture(const std::string &name, const RenderTarget *render_target) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == UNIFORM_INPUT_TYPE_TEXTURE_SAMPLER) {
				setTexture(inputs[it->second].binding, render_target);
			} else {
				Log::error(name + " is not a texture binding");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_PipelineLayout::setTexture(const std::string &name, const Texture *texture) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == UNIFORM_INPUT_TYPE_TEXTURE_SAMPLER) {
				setTexture(inputs[it->second].binding, texture);
			} else {
				Log::error(name + " is not a texture binding");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_PipelineLayout::setTexture(uint32_t binding, const Texture *texture) {
		//todo undefined behavior when texture data is not set;
		//use texture event
		VK_Image *vk_texture = (VK_Image *) texture;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_texture->getImageView();
			image_info.sampler = vk_texture->getSampler();
			image_info.imageLayout = vk_texture->getImageLayout();
			descriptor_sets_writes[i][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}
	}

	void VK_PipelineLayout::setUniform(const std::string &name, const void *data) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == UNIFORM_INPUT_TYPE_BUFFER) {
				setUniform(inputs[it->second].binding, data);

			} else {
				Log::error(name + " is not a uniform buffer");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_PipelineLayout::setUniform(uint32_t binding, const void *data) {
		for (VK_Buffer *buffer: uniform_buffers[binding]) {
			buffer->update(data);
		}
	}

	void VK_PipelineLayout::pushConstant(VkCommandBuffer command_buffer, const std::string &name, const void *data) {
		auto it = name_input_index.find(name);
		if (it != name_input_index.end()) {
			if (inputs[it->second].type == UNIFORM_INPUT_TYPE_PUSH_CONSTANT) {
				vkCmdPushConstants(command_buffer, handle,
								   inputs[it->second].stage, inputs[it->second].offset, inputs[it->second].size, data);
			} else {
				Log::error(name + " is not a push constant");
			}
		} else {
			Log::error("No shader input is named:" + name);
		}
	}

	void VK_PipelineLayout::bindDescriptors(VkCommandBuffer buffer, int frame) {
		vkCmdBindDescriptorSets(buffer,
								VK_PIPELINE_BIND_POINT_GRAPHICS,
								handle,
								0,
								1,
								&descriptor_set_handles[frame],
								0,
								nullptr);
	}


}
