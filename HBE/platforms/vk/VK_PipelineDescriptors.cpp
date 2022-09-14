//
// Created by user on 8/14/2022.
//

#include "VK_PipelineDescriptors.h"
#include "core/resource/Resources.h"
#include "VK_PipelineLayout.h"
#include "VK_RenderPass.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "raytracing/VK_TopLevelAccelerationStructure.h"

namespace HBE {

	VK_PipelineDescriptors::VK_PipelineDescriptors(VK_Renderer *renderer, const VK_PipelineLayout &layout) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->pipeline_layout = &layout;
		descriptor_set_layout_bindings = pipeline_layout->getDescriptorSetLayoutBindings();
		uniform_name_to_index = pipeline_layout->getUniformNameToIndex();
		uniform_binding_to_index = pipeline_layout->getUniformBindingToIndex();
		//todo:Reuse descriptor pools
		createDescriptorPool();
		createDescriptorSets();
	}


	VK_PipelineDescriptors::~VK_PipelineDescriptors() {
		for (auto buffer_vector: uniform_buffers) {
			for (size_t i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		vkDestroyDescriptorPool(device->getHandle(), pool_handle, nullptr);
	}

	void VK_PipelineDescriptors::createDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline_layout->getDescriptorSetLayout());
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool_handle;
		allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, handles.data()) != VK_SUCCESS) {
			Log::error("failed to allocate descriptor sets!");
		}
		for (uint32_t frame_index = 0; frame_index < MAX_FRAMES_IN_FLIGHT; ++frame_index) {

			std::vector<VkWriteDescriptorSet> writes;
			std::vector<VkDescriptorBufferInfo> buffers_info;
			std::vector<VkDescriptorImageInfo> images_info;

			std::vector<VkWriteDescriptorSetAccelerationStructureKHR> acceleration_structure_writes;

			buffers_info.resize(descriptor_set_layout_bindings.size(), {});
			images_info.resize(descriptor_set_layout_bindings.size(), {});
			writes.resize(descriptor_set_layout_bindings.size(), {});
			acceleration_structure_writes.resize(descriptor_set_layout_bindings.size(), {});
			VK_Image *default_texture = (VK_Image *) Resources::get<Texture>("DEFAULT_TEXTURE");
			VK_Image *default_image = (VK_Image *) Resources::get<Texture>("DEFAULT_IMAGE");
			VkAccelerationStructureKHR default_acceleration_structure = VK_NULL_HANDLE;
			for (size_t binding_index = 0; binding_index < descriptor_set_layout_bindings.size(); ++binding_index) {

				writes[binding_index].pBufferInfo = nullptr;
				writes[binding_index].pImageInfo = nullptr;
				uint32_t binding = descriptor_set_layout_bindings[binding_index].binding;
				switch (descriptor_set_layout_bindings[binding_index].descriptorType) {
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
						buffers_info[binding_index].offset = 0;
						auto it = uniform_buffers.find(binding);
						if (it != uniform_buffers.end()) {
							buffers_info[binding_index].buffer = uniform_buffers[binding][frame_index]->getHandle();
							buffers_info[binding_index].range = it->second[frame_index]->getSize();
							writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							writes[binding_index].pBufferInfo = &buffers_info[binding_index];
						} else {
							Log::error("Could not find uniform at binding " + std::to_string(descriptor_set_layout_bindings[binding_index].binding));
						}
						break;
					}
					case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						images_info[binding_index].imageView = default_texture->getImageView(0);
						images_info[binding_index].sampler = default_texture->getSampler();
						images_info[binding_index].imageLayout = default_texture->getImageLayout();
						writes[binding_index].pImageInfo = &images_info[binding_index];
						break;
					case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						images_info[binding_index].imageView = default_image->getImageView(0);
						images_info[binding_index].sampler = VK_NULL_HANDLE;
						images_info[binding_index].imageLayout = default_image->getImageLayout();
						writes[binding_index].pImageInfo = &images_info[binding_index];
						break;
					case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
						images_info[binding_index].imageView = default_texture->getImageView(0);
						images_info[binding_index].sampler = default_texture->getSampler();
						images_info[binding_index].imageLayout = default_texture->getImageLayout();
						writes[binding_index].pImageInfo = &images_info[binding_index];
						break;

					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
						/*If the nullDescriptor feature is enabled, the buffer, acceleration structure, imageView, or bufferView can be VK_NULL_HANDLE. Loads from a null descriptor return zero values and stores and atomics to a null descriptor are discarded. A null acceleration structure descriptor results in the miss shader being invoked.*/
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

						writes[binding_index].pNext = &acceleration_structure_writes[binding_index];

						acceleration_structure_writes[binding_index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
						acceleration_structure_writes[binding_index].accelerationStructureCount = 1;
						acceleration_structure_writes[binding_index].pAccelerationStructures = &default_acceleration_structure;
						break;
					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
					case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE:
					case VK_DESCRIPTOR_TYPE_SAMPLER:
					case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
					case VK_DESCRIPTOR_TYPE_MAX_ENUM:
						Log::error("Descriptor type not supported");
						break;
				}

				writes[binding_index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writes[binding_index].dstSet = handles[frame_index];
				writes[binding_index].dstBinding = descriptor_set_layout_bindings[binding_index].binding;
				writes[binding_index].dstArrayElement = 0;
				writes[binding_index].descriptorCount = 1;//for arrays

				writes[binding_index].pTexelBufferView = nullptr; // Optional
				descriptor_sets_writes[frame_index].emplace(writes[binding_index].dstBinding, writes[binding_index]);
			}
			vkUpdateDescriptorSets(device->getHandle(), writes.size(), writes.data(), 0, nullptr);

		}
	}

	void VK_PipelineDescriptors::bind() const {
		VkCommandBuffer command_buffer = renderer->getCommandPool()->getCurrentBuffer();
		int frame = renderer->getCurrentFrame();
		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								1,
								&handles[frame],
								0,
								nullptr);
	}

	void VK_PipelineDescriptors::bind(VkCommandBuffer command_buffer, uint32_t frame) const {

		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								1,
								&handles[frame],
								0,
								nullptr);
	}

	void VK_PipelineDescriptors::unbind() const {

	}

	void VK_PipelineDescriptors::setTexture(uint32_t binding, const RenderTarget *render_target) {
		VK_RenderPass *render_pass = (VK_RenderPass *) render_target;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			const VK_Image *vk_image = render_pass->getImage(i);
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_image->getImageView(0);
			image_info.sampler = vk_image->getSampler();
			image_info.imageLayout = vk_image->getImageLayout();
			descriptor_sets_writes[i][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setTexture(const std::string &name, const RenderTarget *render_target) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, name + " is not a texture binding");

		setTexture(descriptor_set_layout_bindings[it->second].binding, render_target);
	}

	void VK_PipelineDescriptors::setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, name + " is not a texture binding");

		setTexture(descriptor_set_layout_bindings[it->second].binding, texture, mip_level);
	}

	void VK_PipelineDescriptors::setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) {
		//todo undefined behavior when texture data is not set;
		//use texture event
		VK_Image *vk_texture = (VK_Image *) texture;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_texture->getImageView(mip_level);
			image_info.sampler = vk_texture->getSampler();
			image_info.imageLayout = vk_texture->getImageLayout();
			descriptor_sets_writes[i][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setUniform(const std::string &name, const void *data) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, name + " is not a uniform buffer");

		setUniform(descriptor_set_layout_bindings[it->second].binding, data);

	}

	void VK_PipelineDescriptors::setUniform(uint32_t binding, const void *data) {
		for (VK_Buffer *buffer: uniform_buffers[binding]) {
			buffer->update(data);
		}
	}

	void VK_PipelineDescriptors::createDescriptorPool() {
		std::vector<VkDeviceSize> uniform_sizes = pipeline_layout->getUniformSizes();
		for (size_t i = 0; i < descriptor_set_layout_bindings.size(); ++i) {
			if (descriptor_set_layout_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				uint32_t binding = descriptor_set_layout_bindings[i].binding;
				for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j) {
					uniform_buffers.emplace(binding, std::vector<VK_Buffer *>());
					uniform_buffers[binding].emplace_back(
							new VK_Buffer(device, uniform_sizes[uniform_binding_to_index[binding]], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
										  ALLOC_FLAGS::ALLOC_FLAG_MAPPABLE));
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
			poolSizes[poolSizes.size() - 1].descriptorCount = MAX_FRAMES_IN_FLIGHT;// uniform_buffer_count * MAX_FRAMES_IN_FLIGHT;
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

		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &pool_handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
	}

	void VK_PipelineDescriptors::setDynamicUniform(const std::string &name, const void *data) {
		Log::warning("Dynamic uniform buffers are not implemented yet!");
	}

	void VK_PipelineDescriptors::setDynamicUniform(uint32_t binding, const void *data) {
		Log::warning("Dynamic uniform buffers are not implemented yet!");
	}

	void VK_PipelineDescriptors::setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure *acceleration_structure) {
		const VkAccelerationStructureKHR acceleration_structure_handle = acceleration_structure->getHandle();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			HB_ASSERT(descriptor_set_layout_bindings[uniform_binding_to_index[binding]].descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, "descriptor type mismatch");

			VkWriteDescriptorSetAccelerationStructureKHR accelerationStructureInfo{};
			accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			accelerationStructureInfo.accelerationStructureCount = 1;
			accelerationStructureInfo.pAccelerationStructures = &acceleration_structure_handle;


			descriptor_sets_writes[i][binding].pNext = &accelerationStructureInfo;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setAccelerationStructure(const std::string &name, const VK_TopLevelAccelerationStructure *acceleration_structure) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, name + " is not an acceleration structure");

		setAccelerationStructure(descriptor_set_layout_bindings[it->second].binding, acceleration_structure);
	}


}

