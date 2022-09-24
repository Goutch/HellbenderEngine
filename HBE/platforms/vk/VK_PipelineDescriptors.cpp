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
			std::vector<std::vector<VkDescriptorBufferInfo>> buffers_info;
			std::vector<std::vector<VkDescriptorImageInfo>> images_info;
			std::vector<VkWriteDescriptorSetAccelerationStructureKHR> acceleration_structure_writes;

			buffers_info.resize(descriptor_set_layout_bindings.size(), {});
			images_info.resize(descriptor_set_layout_bindings.size(), {});
			writes.resize(descriptor_set_layout_bindings.size(), {});
			acceleration_structure_writes.resize(descriptor_set_layout_bindings.size(), {});
			VkAccelerationStructureKHR default_acceleration_structure = VK_NULL_HANDLE;
			VkSampler default_sampler = renderer->getDefaultSampler();
			for (size_t binding_index = 0; binding_index < descriptor_set_layout_bindings.size(); ++binding_index) {

				writes[binding_index].pBufferInfo = nullptr;
				writes[binding_index].pImageInfo = nullptr;
				uint32_t binding = descriptor_set_layout_bindings[binding_index].binding;
				uint32_t descriptor_count = descriptor_set_layout_bindings[binding_index].descriptorCount;
				auto descriptor_type = descriptor_set_layout_bindings[binding_index].descriptorType;
				switch (descriptor_type) {
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
						buffers_info[binding_index].resize(descriptor_count, {});
						buffers_info[binding_index][0].offset = 0;
						auto it = uniform_buffers.find(binding);
						HB_ASSERT(it != uniform_buffers.end(), "Could not find uniform at binding " + std::to_string(descriptor_set_layout_bindings[binding_index].binding));
						buffers_info[binding_index][0].buffer = uniform_buffers[binding][frame_index]->getHandle();
						buffers_info[binding_index][0].range = it->second[frame_index]->getSize();
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						writes[binding_index].pBufferInfo = &buffers_info[binding_index][0];
						break;
					}
					case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:

						images_info[binding_index].resize(descriptor_count, {});
						for (int i = 0; i < images_info[binding_index].size(); ++i) {
							images_info[binding_index][i].imageView = VK_NULL_HANDLE;
							images_info[binding_index][i].sampler = default_sampler;
							images_info[binding_index][i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
						}
						writes[binding_index].pImageInfo = &images_info[binding_index][0];
						break;

					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
						writes[binding_index].pNext = &acceleration_structure_writes[binding_index];

						acceleration_structure_writes[binding_index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
						acceleration_structure_writes[binding_index].accelerationStructureCount = 1;
						acceleration_structure_writes[binding_index].pAccelerationStructures = &default_acceleration_structure;
						break;
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
						/*writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						buffers_info[binding_index].offset = 0;
						auto it = uniform_buffers.find(binding);
						HB_ASSERT(it != uniform_buffers.end(), "Could not find uniform at binding " + std::to_string(descriptor_set_layout_bindings[binding_index].binding));
						buffers_info[binding_index].buffer = default_storage_buffer;
						buffers_info[binding_index].range = 0;
						writes[binding_index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						writes[binding_index].pBufferInfo = &buffers_info[binding_index];*/
						break;
					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
					case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE:
					case VK_DESCRIPTOR_TYPE_SAMPLER:
					case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					case VK_DESCRIPTOR_TYPE_MAX_ENUM:
						Log::error("Descriptor type not supported");
						break;
				}

				writes[binding_index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writes[binding_index].dstSet = handles[frame_index];
				writes[binding_index].dstBinding = descriptor_set_layout_bindings[binding_index].binding;
				writes[binding_index].descriptorType = descriptor_type;
				writes[binding_index].dstArrayElement = 0;
				writes[binding_index].descriptorCount = descriptor_count;//for arrays

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

	void VK_PipelineDescriptors::setTextureArray(const std::string &name, const Texture **textures, uint32_t texture_count, uint32_t frame, int32_t mip_level) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, name + " is not a texture binding");

		setTextureArray(descriptor_set_layout_bindings[it->second].binding, textures, texture_count, frame, mip_level);
	}

	void VK_PipelineDescriptors::setTextureArray(uint32_t binding, const Texture **textures, uint32_t texture_count, uint32_t frame, int32_t mip_level) {
		HB_ASSERT(frame < MAX_FRAMES_IN_FLIGHT, "Frame index out of range");
		HB_ASSERT(texture_count == descriptor_set_layout_bindings[binding].descriptorCount, "Too many textures for descriptor");
		const VK_Image **vk_texture = reinterpret_cast<const VK_Image **>(textures);
		std::vector<VkDescriptorImageInfo> image_infos;
		image_infos.resize(texture_count);

		for (uint32_t i = 0; i < texture_count; ++i) {
			image_infos[i].imageLayout = vk_texture[i]->getImageLayout();
			image_infos[i].imageView = vk_texture[i]->getImageView(mip_level);
			image_infos[i].sampler = vk_texture[i]->getSampler();
		}

		if (frame < 0) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				descriptor_sets_writes[i][binding].pImageInfo = image_infos.data();
				vkUpdateDescriptorSets(device->getHandle(),
									   1,
									   &descriptor_sets_writes[i][binding],
									   0,
									   nullptr);
			}
		} else {
			descriptor_sets_writes[frame][binding].pImageInfo = image_infos.data();
			vkUpdateDescriptorSets(device->getHandle(),
								   1,
								   &descriptor_sets_writes[frame][binding],
								   0,
								   nullptr);
		}
	}

	void VK_PipelineDescriptors::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, name + " is not a texture binding");

		setTexture(descriptor_set_layout_bindings[it->second].binding, texture, frame, mip_level);
	}

	void VK_PipelineDescriptors::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {

		VK_Image *vk_texture = (VK_Image *) texture;

		HB_ASSERT(descriptor_sets_writes[0][binding].descriptorCount == 1, "Texture binding is an array");

		if (frame < 0) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

				VkDescriptorImageInfo image_info;
				image_info.imageView = vk_texture->getImageView(mip_level);
				image_info.sampler = vk_texture->getSampler();
				image_info.imageLayout = vk_texture->getImageLayout();
				descriptor_sets_writes[i][binding].pImageInfo = &image_info;
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[i][binding], 0, nullptr);
			}
		} else {
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_texture->getImageView(mip_level);
			image_info.sampler = vk_texture->getSampler();
			image_info.imageLayout = vk_texture->getImageLayout();
			descriptor_sets_writes[frame][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_sets_writes[frame][binding], 0, nullptr);

		}
	}

	void VK_PipelineDescriptors::setUniform(const std::string &name, const void *data, int32_t frame) {
		auto it = uniform_name_to_index.find(name);
		HB_ASSERT(it != uniform_name_to_index.end(), "No shader input is named:" + name);
		HB_ASSERT(descriptor_set_layout_bindings[it->second].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, name + " is not a uniform buffer");

		setUniform(descriptor_set_layout_bindings[it->second].binding, data, frame);
	}

	void VK_PipelineDescriptors::setUniform(uint32_t binding, const void *data, int32_t frame) {
		if (frame < 0) {
			for (VK_Buffer *buffer: uniform_buffers[binding]) {
				buffer->update(data);
			}
		} else {
			uniform_buffers[binding][frame]->update(data);
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
					uniform_buffer_count += layout_binding.descriptorCount;
					break;
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					combined_image_sampler_count += layout_binding.descriptorCount;
					break;
				case VK_DESCRIPTOR_TYPE_SAMPLER:
					break;
				case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					separate_image_count += layout_binding.descriptorCount;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					storage_image_count += layout_binding.descriptorCount;
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

		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &pool_handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
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

