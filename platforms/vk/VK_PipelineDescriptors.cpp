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
#include "VK_StorageBuffer.h"
#include "core/graphics/Graphics.h"
#include "VK_TexelBuffer.h"
#include "core/utility/Profiler.h"
#include "VK_TexelBuffer.h"

namespace HBE {

	VK_PipelineDescriptors::VK_PipelineDescriptors(VK_Renderer *renderer, const VK_PipelineLayout &layout) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->pipeline_layout = &layout;
		descriptor_set_layout_bindings = pipeline_layout->getDescriptorSetLayoutBindings();

		std::vector<VkDeviceSize> descriptor_sizes = pipeline_layout->getDescriptorSizes();
		for (size_t i = 0; i < descriptor_set_layout_bindings.size(); ++i) {
			if (descriptor_set_layout_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				uint32_t binding = descriptor_set_layout_bindings[i].binding;
				for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j) {
					uniform_buffers.emplace(binding, std::vector<VK_Buffer *>());
					uniform_buffers[binding].emplace_back(
							new VK_Buffer(device, descriptor_sizes[binding], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
										  ALLOC_FLAG_MAPPABLE));
				}
			}
		}

		createDescriptorPool(descriptor_pool);
		createDescriptorWrites(descriptor_pool);
		Graphics::onFrameChange.subscribe(this, &VK_PipelineDescriptors::onFrameChange);
	}


	VK_PipelineDescriptors::~VK_PipelineDescriptors() {
		for (auto buffer_vector: uniform_buffers) {
			for (size_t i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		vkDestroyDescriptorPool(device->getHandle(), descriptor_pool.handle, nullptr);
	}


	void VK_PipelineDescriptors::copyDescriptorSets(DescriptorPool &from, DescriptorPool &to, int32_t frame) {

		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				copyDescriptorSets(from, to, i);
			}
		} else {
			std::vector<VkCopyDescriptorSet> copyDescriptorSet;

			for (auto it_write: from.writes[frame]) {
				uint32_t binding = it_write.dstBinding;
				VkWriteDescriptorSet &from_write = from.writes[frame][binding];
				VkWriteDescriptorSet &to_write = to.writes[frame][binding];

				VkCopyDescriptorSet copy{};
				copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
				copy.srcSet = from_write.dstSet;
				copy.dstSet = to_write.dstSet;
				copy.srcBinding = from_write.dstBinding;
				copy.dstBinding = to_write.dstBinding;
				copy.descriptorCount = from_write.descriptorCount;
				if (copy.descriptorCount > 0) {
					copyDescriptorSet.emplace_back(copy);
				}
			}
			vkUpdateDescriptorSets(device->getHandle(), 0, nullptr, copyDescriptorSet.size(), copyDescriptorSet.data());
		}

	}

	void VK_PipelineDescriptors::createDescriptorWrites(DescriptorPool &pool) {
		for (uint32_t frame_index = 0; frame_index < MAX_FRAMES_IN_FLIGHT; ++frame_index) {
			std::vector<VkWriteDescriptorSet> uniform_writes;
			for (size_t binding_index = 0; binding_index < descriptor_set_layout_bindings.size(); ++binding_index) {
				auto descriptor_type = descriptor_set_layout_bindings[binding_index].descriptorType;
				auto binding = descriptor_set_layout_bindings[binding_index].binding;
				uint32_t descriptor_count = descriptor_set_layout_bindings[binding_index].descriptorCount;
				if (pipeline_layout->IsBindingVariableSize(binding)) {
					auto it = pool.variable_descriptors.find(binding);
					if (it != pool.variable_descriptors.end()) {
						descriptor_count = it->second.count;
					} else {
						descriptor_count = 0;
					}
				}
				VkWriteDescriptorSet write = {};
				write.descriptorType = descriptor_type;
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.dstSet = pool.set_handles[frame_index];
				write.dstBinding = binding;
				write.descriptorType = descriptor_type;
				write.dstArrayElement = 0;
				write.descriptorCount = descriptor_count;//for arrays
				write.pTexelBufferView = nullptr; // Optional


				pool.writes[frame_index].emplace_back(write);

				if (descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					uniform_writes.emplace_back(write);
				}
			}
			//set uniform buffers
			if (uniform_writes.size() > 0) {
				std::vector<VkDescriptorBufferInfo> uniform_buffer_infos(uniform_writes.size());
				for (int i = 0; i < uniform_writes.size(); ++i) {
					VkDescriptorBufferInfo buffer_info = {};
					buffer_info.buffer = uniform_buffers[uniform_writes[i].dstBinding][frame_index]->getHandle();
					buffer_info.offset = 0;
					buffer_info.range = uniform_buffers[uniform_writes[i].dstBinding][frame_index]->getSize();
					uniform_buffer_infos[i] = buffer_info;
					uniform_writes[i].pBufferInfo = &uniform_buffer_infos[i];
				}
				vkUpdateDescriptorSets(device->getHandle(), uniform_writes.size(), uniform_writes.data(), 0, nullptr);
			}
		}
	}

	void VK_PipelineDescriptors::createDescriptorPool(DescriptorPool &pool) {
		size_t combined_image_sampler_count = 0;
		size_t separate_image_count = 0;
		size_t storage_image_count = 0;
		size_t uniform_buffer_count = 0;
		size_t storage_buffer_count = 0;
		size_t storage_texel_buffer_count = 0;
		size_t uniform_texel_buffer_count = 0;
		for (auto layout_binding: descriptor_set_layout_bindings) {
			if (pipeline_layout->IsBindingVariableSize(layout_binding.binding)) {
				continue;
			}
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
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					storage_buffer_count += layout_binding.descriptorCount;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					storage_texel_buffer_count += layout_binding.descriptorCount;
					break;
				case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					uniform_texel_buffer_count += layout_binding.descriptorCount;
					break;
			}
		}
		for (auto it: pool.variable_descriptors) {
			VariableDescriptor &variable_descriptor = it.second;
			switch (variable_descriptor.type) {
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					uniform_buffer_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					combined_image_sampler_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					separate_image_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					storage_image_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					storage_buffer_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					storage_texel_buffer_count += variable_descriptor.count;
					break;
				case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					uniform_texel_buffer_count += variable_descriptor.count;
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
		if (storage_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_buffer_count * MAX_FRAMES_IN_FLIGHT;
		}
		if (storage_texel_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_texel_buffer_count * MAX_FRAMES_IN_FLIGHT;
		}
		if (uniform_texel_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = uniform_texel_buffer_count * MAX_FRAMES_IN_FLIGHT;
		}

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &pool.handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}


		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline_layout->getDescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool.handle;
		allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();
		VkDescriptorSetVariableDescriptorCountAllocateInfo variable_count_info{};
		if (pool.variable_descriptors.size() > 0) {
			std::array<uint32_t, MAX_FRAMES_IN_FLIGHT> sizes;
			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				sizes[i] = 0;
				for (auto it: pool.variable_descriptors) {
					VariableDescriptor &variable_descriptor = it.second;
					sizes[i] += variable_descriptor.count;
				}
			}
			variable_count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
			variable_count_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
			variable_count_info.pDescriptorCounts = sizes.data();
			allocInfo.pNext = &variable_count_info;
		}

		if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, pool.set_handles.data()) != VK_SUCCESS) {
			Log::error("failed to allocate descriptor sets!");
		}
	}

	void VK_PipelineDescriptors::bind() const {
		if (bound) return;
		VkCommandBuffer command_buffer = renderer->getCommandPool()->getCurrentBuffer();
		int frame = renderer->getCurrentFrame();
		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								1,
								&descriptor_pool.set_handles[frame],
								0,
								nullptr);
	}

	void VK_PipelineDescriptors::bind(VkCommandBuffer command_buffer, uint32_t frame) const {
		if (bound) return;
		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								1,
								&descriptor_pool.set_handles[frame],
								0,
								nullptr);
	}

	void VK_PipelineDescriptors::unbind() const {
		bound = false;
	}

	uint32_t VK_PipelineDescriptors::getBinding(const std::string &name) const {
		return pipeline_layout->getDescriptorBinding(name);
	}

	void VK_PipelineDescriptors::setTextureArray(uint32_t binding, Texture **textures, uint32_t texture_count, int32_t frame, int32_t mip_level) {

		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VkDescriptorSetLayoutBinding &binding_info = descriptor_set_layout_bindings[binding];
		HB_ASSERT(binding_info.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  binding_info.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  binding_info.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, "binding#" + std::to_string(binding) + " is not a texture");
		//variable descriptor count
		if (pipeline_layout->IsBindingVariableSize(binding)) {
			createVariableSizeDescriptors(binding, binding_info.descriptorType, texture_count, frame);
		}


		VK_Image **vk_texture = reinterpret_cast<VK_Image **>(textures);
		std::vector<VkDescriptorImageInfo> image_infos;

		VkWriteDescriptorSet write_descriptor_set = descriptor_pool.writes[frame][binding];

		image_infos.resize(write_descriptor_set.descriptorCount);
		for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; ++i) {
			int index = i >= texture_count ? texture_count - 1 : i;

			image_infos[i].imageLayout = vk_texture[index]->getImageLayout();
			image_infos[i].imageView = vk_texture[index]->getImageView(mip_level);
			image_infos[i].sampler = vk_texture[index]->getSampler();
		}


		if (frame == -1) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				write_descriptor_set.pImageInfo = image_infos.data();
				vkUpdateDescriptorSets(device->getHandle(),
									   1,
									   &write_descriptor_set,
									   0,
									   nullptr);
			}
		} else {
			write_descriptor_set.pImageInfo = image_infos.data();
			vkUpdateDescriptorSets(device->getHandle(),
								   1,
								   &write_descriptor_set,
								   0,
								   nullptr);
		}
	}

	void VK_PipelineDescriptors::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {

		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VK_Image *vk_texture = (VK_Image *) texture;

		HB_ASSERT(descriptor_pool.writes[0][binding].descriptorCount == 1, "Texture binding is an array");

		VkWriteDescriptorSet write{};
		write.descriptorCount = 1;
		if (frame < 0) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

				VkDescriptorImageInfo image_info;
				image_info.imageView = vk_texture->getImageView(mip_level);
				image_info.sampler = vk_texture->getSampler();
				image_info.imageLayout = vk_texture->getImageLayout();
				descriptor_pool.writes[i][binding].pImageInfo = &image_info;
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			VkDescriptorImageInfo image_info;
			image_info.imageView = vk_texture->getImageView(mip_level);
			image_info.sampler = vk_texture->getSampler();
			image_info.imageLayout = vk_texture->getImageLayout();
			descriptor_pool.writes[frame][binding].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setUniform(uint32_t binding, const void *data, int32_t frame) {
		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		HB_ASSERT(descriptor_set_layout_bindings[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				  "binding#" + std::to_string(binding) + " is not a uniform buffer");
		if (frame < 0) {
			for (VK_Buffer *buffer: uniform_buffers[binding]) {
				buffer->update(data);
			}
		} else {
			uniform_buffers[binding][frame]->update(data);
		}
	}


	void VK_PipelineDescriptors::setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure *acceleration_structure, int32_t frame) {
		HB_ASSERT(frame <= int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");

		VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = descriptor_set_layout_bindings[binding];
		const VkAccelerationStructureKHR acceleration_structure_handle = acceleration_structure->getHandle();
		if (frame == -1) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, "binding#" + std::to_string(binding) + " is not an acceleration structure");

				VkWriteDescriptorSetAccelerationStructureKHR accelerationStructureInfo{};
				accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
				accelerationStructureInfo.accelerationStructureCount = 1;
				accelerationStructureInfo.pAccelerationStructures = &acceleration_structure_handle;


				descriptor_pool.writes[i][binding].pNext = &accelerationStructureInfo;

				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, "binding#" + std::to_string(binding) + " is not an acceleration structure");

			VkWriteDescriptorSetAccelerationStructureKHR accelerationStructureInfo{};
			accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			accelerationStructureInfo.accelerationStructureCount = 1;
			accelerationStructureInfo.pAccelerationStructures = &acceleration_structure_handle;

			descriptor_pool.writes[frame][binding].pNext = &accelerationStructureInfo;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);

		}
	}

	void VK_PipelineDescriptors::createVariableSizeDescriptors(uint32_t binding, VkDescriptorType descriptor_type, uint32_t count, int32_t frame) {
		auto it_variable_descriptor = descriptor_pool.variable_descriptors.find(binding);
		if (it_variable_descriptor == descriptor_pool.variable_descriptors.end()) {
			VariableDescriptor variable_descriptor{};
			variable_descriptor.binding = binding;
			variable_descriptor.count = 0;
			variable_descriptor.type = descriptor_type;
			descriptor_pool.variable_descriptors.emplace(binding, variable_descriptor);
		}

		if (descriptor_pool.variable_descriptors[binding].count != count) {
			VariableDescriptor variable_descriptor{};
			variable_descriptor.binding = binding;
			variable_descriptor.count = count;
			variable_descriptor.type = descriptor_type;
			auto it = temp_descriptor_pool.variable_descriptors.find(binding);
			if (it == temp_descriptor_pool.variable_descriptors.end()) {
				temp_descriptor_pool.variable_descriptors.emplace(binding, variable_descriptor);
			} else {
				temp_descriptor_pool.variable_descriptors[binding] = variable_descriptor;
			}
			createDescriptorPool(temp_descriptor_pool);
			createDescriptorWrites(temp_descriptor_pool);
			copyDescriptorSets(descriptor_pool, temp_descriptor_pool, frame);

			old_descriptor_pools.push(std::make_pair(renderer->getCurrentFrame(), descriptor_pool.handle));

			descriptor_pool.handle = temp_descriptor_pool.handle;
			descriptor_pool.variable_descriptors = temp_descriptor_pool.variable_descriptors;
			descriptor_pool.writes = temp_descriptor_pool.writes;
			descriptor_pool.set_handles = temp_descriptor_pool.set_handles;
		}
	}

	void VK_PipelineDescriptors::setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame) {

		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = descriptor_set_layout_bindings[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");
		HB_ASSERT(count <= descriptorSetLayoutBinding.descriptorCount || descriptorSetLayoutBinding.descriptorCount == 0, "descriptor count mismatch");

		VK_StorageBuffer **vk_buffers = reinterpret_cast<VK_StorageBuffer **>(buffers);
		//variable descriptor count
		if (pipeline_layout->IsBindingVariableSize(binding)) {
			createVariableSizeDescriptors(binding, descriptorSetLayoutBinding.descriptorType, count, frame);
		}

		std::vector<VkDescriptorBufferInfo> buffer_infos(count);
		for (int i = 0; i < count; i++) {
			buffer_infos[i].buffer = vk_buffers[i]->getBuffer().getHandle();
			buffer_infos[i].offset = 0;
			buffer_infos[i].range = VK_WHOLE_SIZE;
		}

		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {


				descriptor_pool.writes[i][binding].descriptorCount = count;
				descriptor_pool.writes[i][binding].pBufferInfo = buffer_infos.data();
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			descriptor_pool.writes[frame][binding].pBufferInfo = buffer_infos.data();
			descriptor_pool.writes[frame][binding].descriptorCount = count;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);

		}
	}

	void VK_PipelineDescriptors::setStorageBuffer(uint32_t binding, StorageBuffer *buffer, int32_t frame) {
		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = descriptor_set_layout_bindings[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");

		VK_StorageBuffer *vk_storage_buffer = dynamic_cast< VK_StorageBuffer *>(buffer);
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = vk_storage_buffer->getBuffer().getHandle();
		buffer_info.offset = 0;
		buffer_info.range = VK_WHOLE_SIZE;

		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				descriptor_pool.writes[i][binding].pBufferInfo = &buffer_info;
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			descriptor_pool.writes[frame][binding].pBufferInfo = &buffer_info;
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setTexelBuffer(uint32_t binding, const TexelBuffer *buffer, int32_t frame) {
		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = descriptor_set_layout_bindings[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");

		const VK_TexelBuffer *vk_texel_buffer = dynamic_cast<const VK_TexelBuffer *>(buffer);

		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				descriptor_pool.writes[i][binding].pTexelBufferView = &vk_texel_buffer->getView();
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			descriptor_pool.writes[frame][binding].pTexelBufferView = &vk_texel_buffer->getView();
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);
		}
	}

	void VK_PipelineDescriptors::setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t buffer_count, int32_t frame) {
		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = descriptor_set_layout_bindings[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");
		HB_ASSERT(buffer_count <= descriptorSetLayoutBinding.descriptorCount || descriptorSetLayoutBinding.descriptorCount == 0, "descriptor count mismatch");

		VK_TexelBuffer **vk_buffers = reinterpret_cast<VK_TexelBuffer **>(buffers);
		//variable descriptor count
		if (pipeline_layout->IsBindingVariableSize(binding)) {
			createVariableSizeDescriptors(binding, descriptorSetLayoutBinding.descriptorType, buffer_count, frame);
		}

		std::vector<VkBufferView> buffer_views(buffer_count);
		for (int i = 0; i < buffer_count; i++) {
			buffer_views[i] = vk_buffers[i]->getView();
		}

		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				descriptor_pool.writes[frame][binding].descriptorCount = buffer_views.size();
				descriptor_pool.writes[frame][binding].pTexelBufferView = buffer_views.data();
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
			}
		} else {
			descriptor_pool.writes[frame][binding].descriptorCount = buffer_views.size();
			descriptor_pool.writes[frame][binding].pTexelBufferView = buffer_views.data();
			vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);

		}
	}

	void VK_PipelineDescriptors::onFrameChange(uint32_t frame) {
		while (!old_descriptor_pools.empty() && old_descriptor_pools.front().first == frame) {
			vkDestroyDescriptorPool(device->getHandle(), old_descriptor_pools.front().second, nullptr);
			old_descriptor_pools.pop();
		}
	}
}

