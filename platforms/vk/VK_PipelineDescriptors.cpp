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
#include "Application.h"

namespace HBE {

	VK_PipelineDescriptors::VK_PipelineDescriptors(VK_Renderer *renderer, const VK_PipelineLayout &layout) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->pipeline_layout = &layout;

		const std::vector<VkDeviceSize> descriptor_sizes = pipeline_layout->getDescriptorSizes();
		const std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings = pipeline_layout->getDescriptorBindings();

		for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame) {
			uniform_buffers[frame].resize(descriptor_bindings.size(), nullptr);
			for (size_t binding = 0; binding < descriptor_bindings.size(); ++binding) {
				if (descriptor_bindings[binding].descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					continue;

				uniform_buffers[frame][binding] = new VK_Buffer(device,
																descriptor_sizes[binding],
																VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
																ALLOC_FLAG_MAPPABLE);
			}
		}
		createDescriptorPool(descriptor_pool);
		createDescriptorWrites(descriptor_pool);
		Graphics::onFrameChange.subscribe(this, &VK_PipelineDescriptors::onFrameChange);
	}


	VK_PipelineDescriptors::~VK_PipelineDescriptors() {
		for (const auto &buffers_per_frame: uniform_buffers) {
			for (auto b: buffers_per_frame) {
				delete b;
			}
		}
		vkDestroyDescriptorPool(device->getHandle(), descriptor_pool.handle, nullptr);
	}

	void VK_PipelineDescriptors::resetPool(DescriptorPool &pool) {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			pool.writes[i].clear();
		}
		pool.handle = VK_NULL_HANDLE;
		pool.descriptor_set_handles.clear();
		pool.variable_descriptor_sets.clear();
	}

	void VK_PipelineDescriptors::createDescriptorPool(DescriptorPool &pool) {

		const std::vector<VkDescriptorSetLayout> pipeline_descriptor_set_layouts = pipeline_layout->getDescriptorSetLayoutHandles();
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts(MAX_FRAMES_IN_FLIGHT * pipeline_descriptor_set_layouts.size());
		for (int i = 0; i < descriptor_set_layouts.size(); ++i) {
			descriptor_set_layouts[i] = pipeline_descriptor_set_layouts[i % pipeline_descriptor_set_layouts.size()];
		}

		size_t combined_image_sampler_count = 0;
		size_t separate_image_count = 0;
		size_t storage_image_count = 0;
		size_t uniform_buffer_count = 0;
		size_t storage_buffer_count = 0;
		size_t storage_texel_buffer_count = 0;
		size_t uniform_texel_buffer_count = 0;
		size_t acceleration_structure_count = 0;

		for (auto layout_binding: pipeline_layout->getDescriptorBindings()) {
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
				case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					acceleration_structure_count += layout_binding.descriptorCount;
					break;
			}
		}
		if (pool.variable_descriptor_sets.size() < descriptor_set_layouts.size()) {
			pool.variable_descriptor_sets.resize(descriptor_set_layouts.size(), {});
		}
		bool has_variable_size_descriptors = false;
		std::vector<uint32_t> variable_sizes(pool.variable_descriptor_sets.size(), 0);
		for (int i = 0; i < pool.variable_descriptor_sets.size(); ++i) {
			VariableDescriptorSet &variable_descriptor = pool.variable_descriptor_sets[i];
			variable_descriptor.binding = pipeline_layout->getLastDescriptorSetBinding(i % pipeline_layout->getDescriptorSetCount());
			variable_descriptor.type = pipeline_layout->getDescriptorInfos()[pool.variable_descriptor_sets[i].binding].layout_binding.descriptorType;

			if (pipeline_layout->IsBindingVariableSize(variable_descriptor.binding)) {
				has_variable_size_descriptors = true;
				variable_sizes[i] += variable_descriptor.count;
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
					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
						acceleration_structure_count += variable_descriptor.count;
						break;
				}
			}
		}
		std::vector<VkDescriptorPoolSize> poolSizes{};
		if (uniform_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = uniform_buffer_count;
		}
		if (combined_image_sampler_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[poolSizes.size() - 1].descriptorCount = combined_image_sampler_count;
		}
		if (storage_image_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_image_count;
		}
		if (separate_image_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			poolSizes[poolSizes.size() - 1].descriptorCount = separate_image_count;
		}
		if (storage_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_buffer_count;
		}
		if (storage_texel_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = storage_texel_buffer_count;
		}
		if (uniform_texel_buffer_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			poolSizes[poolSizes.size() - 1].descriptorCount = uniform_texel_buffer_count;
		}
		if (acceleration_structure_count > 0) {
			poolSizes.emplace_back();
			poolSizes[poolSizes.size() - 1].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			poolSizes[poolSizes.size() - 1].descriptorCount = acceleration_structure_count;
		}


		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = descriptor_set_layouts.size();
		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &pool.handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
		pool.descriptor_set_handles.resize(descriptor_set_layouts.size());
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool.handle;
		allocInfo.descriptorSetCount = descriptor_set_layouts.size();
		allocInfo.pSetLayouts = descriptor_set_layouts.data();

		VkDescriptorSetVariableDescriptorCountAllocateInfo variable_count_info{};
		if (has_variable_size_descriptors) {
			bool descriptor_indexing_enabled = Application::getInfo().required_extension_flags & VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING;
			HB_ASSERT(has_variable_size_descriptors == descriptor_indexing_enabled, "Descriptor indexing is not enabled but variable size descriptors are used!");
			variable_count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
			variable_count_info.descriptorSetCount = allocInfo.descriptorSetCount;
			variable_count_info.pDescriptorCounts = variable_sizes.data();
			allocInfo.pNext = &variable_count_info;
		}


		if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, pool.descriptor_set_handles.data()) != VK_SUCCESS) {
			Log::error("failed to allocate descriptor sets!");
		}
	}

	void VK_PipelineDescriptors::copyDescriptorSets(DescriptorPool &from, DescriptorPool &to, int32_t frame) {
		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				copyDescriptorSets(from, to, i);
			}
		} else {
			std::vector<VkCopyDescriptorSet> descriptor_set_copy_infos;

			for (auto write: from.writes[frame]) {
				uint32_t binding = write.dstBinding;
				VkWriteDescriptorSet &from_write = from.writes[frame][binding];
				VkWriteDescriptorSet &to_write = to.writes[frame][binding];

				VkCopyDescriptorSet copy{};
				copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
				copy.srcSet = from_write.dstSet;
				copy.dstSet = to_write.dstSet;
				copy.srcBinding = from_write.dstBinding;
				copy.dstBinding = to_write.dstBinding;
				copy.descriptorCount = std::min(from_write.descriptorCount, to_write.descriptorCount);
				if (copy.descriptorCount > 0) {
					descriptor_set_copy_infos.emplace_back(copy);
				}
			}
			vkUpdateDescriptorSets(device->getHandle(), 0, nullptr, descriptor_set_copy_infos.size(), descriptor_set_copy_infos.data());
		}
	}

	void VK_PipelineDescriptors::createDescriptorWrites(DescriptorPool &pool) {
		const std::vector<VkDescriptorSetLayoutBinding> layout_bindings = pipeline_layout->getDescriptorBindings();
		const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts = pipeline_layout->getDescriptorSetLayoutHandles();
		const std::vector<VK_DescriptorInfo> &descriptor_infos = pipeline_layout->getDescriptorInfos();

		for (uint32_t frame_index = 0; frame_index < MAX_FRAMES_IN_FLIGHT; ++frame_index) {
			std::vector<VkWriteDescriptorSet> descriptor_writes;
			for (size_t binding = 0; binding < layout_bindings.size(); ++binding) {
				auto descriptor_type = layout_bindings[binding].descriptorType;
				uint32_t descriptor_count = layout_bindings[binding].descriptorCount;
				uint32_t descriptor_set_index = (frame_index * descriptor_set_layouts.size()) + descriptor_infos[binding].descriptor_set;

				if (pipeline_layout->IsBindingVariableSize(binding)) {
					descriptor_count = pool.variable_descriptor_sets[descriptor_set_index].count;
				}

				VkWriteDescriptorSet write = {};
				write.descriptorType = descriptor_type;
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.dstSet = pool.descriptor_set_handles[descriptor_set_index];
				write.dstBinding = binding;
				write.descriptorType = descriptor_type;
				write.dstArrayElement = 0;
				write.descriptorCount = descriptor_count;
				write.pTexelBufferView = nullptr; // Optional


				pool.writes[frame_index].emplace_back(write);

				if (descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					descriptor_writes.emplace_back(write);
				}
			}
			//set uniform buffers
			if (descriptor_writes.size() > 0) {
				std::vector<VkDescriptorBufferInfo> uniform_buffer_infos(descriptor_writes.size());
				for (int i = 0; i < descriptor_writes.size(); ++i) {
					if (descriptor_writes[i].descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
						continue;
					}
					VkDescriptorBufferInfo buffer_info = {};
					buffer_info.buffer = uniform_buffers[frame_index][descriptor_writes[i].dstBinding]->getHandle();
					buffer_info.offset = 0;
					buffer_info.range = uniform_buffers[frame_index][descriptor_writes[i].dstBinding]->getSize();
					uniform_buffer_infos[i] = buffer_info;
					descriptor_writes[i].pBufferInfo = &uniform_buffer_infos[i];
				}
				vkUpdateDescriptorSets(device->getHandle(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
			}
		}
	}

	void VK_PipelineDescriptors::createVariableSizeDescriptors(uint32_t binding, VkDescriptorType descriptor_type, uint32_t count, int32_t frame) {
		if (frame == -1) {
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				createVariableSizeDescriptors(binding, descriptor_type, count, i);
			}
			return;
		}

		uint32_t descriptor_set = pipeline_layout->getDescriptorInfos()[binding].descriptor_set;
		uint32_t last_binding = pipeline_layout->getLastDescriptorSetBinding(descriptor_set);


		uint32_t descriptor_set_index = (frame * pipeline_layout->getDescriptorSetLayoutHandles().size()) + descriptor_set;

		if (descriptor_pool.variable_descriptor_sets[descriptor_set_index].count != count) {
			VariableDescriptorSet variable_descriptor{};
			variable_descriptor.binding = binding;
			variable_descriptor.count = count;
			variable_descriptor.type = descriptor_type;

			resetPool(temp_descriptor_pool);

			temp_descriptor_pool.variable_descriptor_sets = std::move(descriptor_pool.variable_descriptor_sets);
			temp_descriptor_pool.variable_descriptor_sets[descriptor_set_index] = variable_descriptor;

			createDescriptorPool(temp_descriptor_pool);
			createDescriptorWrites(temp_descriptor_pool);
			copyDescriptorSets(descriptor_pool, temp_descriptor_pool, frame);

			old_descriptor_pools.emplace(renderer->getCurrentFrame(), descriptor_pool.handle);

			descriptor_pool.handle = temp_descriptor_pool.handle;
			descriptor_pool.variable_descriptor_sets = std::move(temp_descriptor_pool.variable_descriptor_sets);
			descriptor_pool.writes = std::move(temp_descriptor_pool.writes);
			descriptor_pool.descriptor_set_handles = std::move(temp_descriptor_pool.descriptor_set_handles);
		}
	}

	void VK_PipelineDescriptors::bind() const {
		if (bound) return;
		VkCommandBuffer command_buffer = renderer->getCommandPool()->getCurrentBuffer();
		int frame = renderer->getCurrentFrame();

		uint32_t descriptor_set_count = pipeline_layout->getDescriptorSetLayoutHandles().size();
		uint32_t offset = descriptor_set_count * frame;

		const VkDescriptorSet *descriptor_sets = descriptor_pool.descriptor_set_handles.data() + offset;

		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								descriptor_set_count,
								descriptor_sets,
								0,
								nullptr);
		bound = true;
	}

	void VK_PipelineDescriptors::bind(VkCommandBuffer command_buffer, uint32_t frame) const {
		if (bound) return;

		uint32_t descriptor_set_count = pipeline_layout->getDescriptorSetLayoutHandles().size();
		uint32_t offset = descriptor_set_count * frame;
		const VkDescriptorSet *descriptor_sets = descriptor_pool.descriptor_set_handles.data() + offset;
		vkCmdBindDescriptorSets(command_buffer,
								pipeline_layout->getBindPoint(),
								pipeline_layout->getHandle(),
								0,
								descriptor_set_count,
								descriptor_sets,
								0,
								nullptr);
		bound = true;
	}

	void VK_PipelineDescriptors::unbind() const {
		bound = false;
	}

	uint32_t VK_PipelineDescriptors::getBinding(const std::string &name) const {
		return pipeline_layout->getDescriptorBinding(name);
	}

	void VK_PipelineDescriptors::setTextureArray(uint32_t binding, Texture **textures, uint32_t texture_count, int32_t frame, int32_t mip_level) {

		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		const VkDescriptorSetLayoutBinding &binding_info = pipeline_layout->getDescriptorBindings()[binding];
		HB_ASSERT(binding_info.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				  binding_info.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
				  binding_info.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, "binding#" + std::to_string(binding) + " is not a texture");
		//variable descriptor count
		if (pipeline_layout->IsBindingVariableSize(binding)) {
			createVariableSizeDescriptors(binding, binding_info.descriptorType, texture_count, frame);
		}


		VK_Image **vk_texture = reinterpret_cast<VK_Image **>(textures);
		std::vector<VkDescriptorImageInfo> image_infos;


		if (frame == -1) {
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
				VkWriteDescriptorSet write_descriptor_set = descriptor_pool.writes[i][binding];

				image_infos.resize(write_descriptor_set.descriptorCount);
				for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; ++i) {
					int index = i >= texture_count ? texture_count - 1 : i;

					image_infos[i].imageLayout = vk_texture[index]->getImageLayout();
					image_infos[i].imageView = vk_texture[index]->getImageView(mip_level);
					image_infos[i].sampler = vk_texture[index]->getSampler();
				}
				write_descriptor_set.pImageInfo = image_infos.data();
				vkUpdateDescriptorSets(device->getHandle(),
									   1,
									   &write_descriptor_set,
									   0,
									   nullptr);
			}
		} else {
			VkWriteDescriptorSet write_descriptor_set = descriptor_pool.writes[frame][binding];

			image_infos.resize(write_descriptor_set.descriptorCount);
			for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; ++i) {
				int index = i >= texture_count ? texture_count - 1 : i;

				image_infos[i].imageLayout = vk_texture[index]->getImageLayout();
				image_infos[i].imageView = vk_texture[index]->getImageView(mip_level);
				image_infos[i].sampler = vk_texture[index]->getSampler();
			}
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
		HB_ASSERT(pipeline_layout->getDescriptorBindings()[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				  "binding#" + std::to_string(binding) + " is not a uniform buffer");
		if (frame < 0) {
			for (int frame_i = 0; frame_i < MAX_FRAMES_IN_FLIGHT; ++frame_i) {
				uniform_buffers[frame_i][binding]->update(data);
			}
		} else {
			uniform_buffers[frame][binding]->update(data);
		}
	}


	void VK_PipelineDescriptors::setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure *acceleration_structure, int32_t frame) {
		HB_ASSERT(frame <= int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");

		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
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

	void VK_PipelineDescriptors::setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame) {

		HB_ASSERT(frame < int32_t(MAX_FRAMES_IN_FLIGHT), "Frame index out of range");
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
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
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
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
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
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
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
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
		if (buffer_count != 0) {
			if (frame == -1) {
				for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
					descriptor_pool.writes[i][binding].descriptorCount = buffer_views.size();
					descriptor_pool.writes[i][binding].pTexelBufferView = buffer_views.data();
					vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[i][binding], 0, nullptr);
				}
			} else {
				descriptor_pool.writes[frame][binding].descriptorCount = buffer_views.size();
				descriptor_pool.writes[frame][binding].pTexelBufferView = buffer_views.data();
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptor_pool.writes[frame][binding], 0, nullptr);
			}
		}

	}

	void VK_PipelineDescriptors::onFrameChange(uint32_t frame) {
		while (!old_descriptor_pools.empty() && old_descriptor_pools.front().first == frame) {
			vkDestroyDescriptorPool(device->getHandle(), old_descriptor_pools.front().second, nullptr);
			old_descriptor_pools.pop();
		}
	}


}

