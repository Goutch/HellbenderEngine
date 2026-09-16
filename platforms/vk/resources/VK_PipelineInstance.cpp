#include "VK_PipelineInstance.h"
#include "VK_RasterizationPipeline.h"
#include "core/utility/Log.h"
#include "platforms/vk/VK_Context.h"
#include "utils-collection/Profiler.h"

namespace HBE {
	inline PIPELINE_INSTANCE_TYPE VK_PipelineInstance::getType() {
		return pipeline_type;
	}

	void VK_PipelineInstance::alloc(VK_Context *context,
	                                const PipelineInstanceInfo &info) {
		this->context = context;
		this->pipeline_type = info.type;
		VkPipeline vk_pipeline_handle = VK_NULL_HANDLE;
		pipeline_handle = info.pipeline_handle;
		switch (info.type) {
			case PIPELINE_INSTANCE_TYPE_RASTERIZATION:
				pipeline_layout = &context->rasterization_pipelines[info.pipeline_handle].getPipelineLayout();
				vk_pipeline_handle = context->rasterization_pipelines[info.pipeline_handle].getHandle();
				break;
			case PIPELINE_INSTANCE_TYPE_RAY_TRACING:
				pipeline_layout = &context->raytracing_pipelines[info.pipeline_handle].getPipelineLayout();
				vk_pipeline_handle = context->raytracing_pipelines[info.pipeline_handle].getHandle();
				break;
			case PIPELINE_INSTANCE_TYPE_COMPUTE:
				pipeline_layout = &context->compute_pipelines[info.pipeline_handle].getPipelineLayout();
				vk_pipeline_handle = context->compute_pipelines[info.pipeline_handle].getHandle();
				break;
			default:
				Log::error("PipelineInstanceInfo.type must not be PIPELINE_INSTANCE_TYPE_NONE");
				break;
		}

		const std::vector<VkDescriptorSetLayoutBinding> &descriptor_bindings = pipeline_layout->getDescriptorBindings();

		std::unordered_map<uint32_t, UniformMemoryInfo> binding_memory_type_map;
		for (uint32_t i = 0; i < info.uniform_memory_type_info_count; ++i) {
			if (info.uniform_memory_type_infos[i].name != "") {
				info.uniform_memory_type_infos[i].binding = pipeline_layout->getDescriptorBinding(info.uniform_memory_type_infos[i].name.c_str());
			}
			binding_memory_type_map.emplace(info.uniform_memory_type_infos[i].binding, info.uniform_memory_type_infos[i]);
		}

		uniform_buffers.resize(descriptor_bindings.size() * MAX_FRAMES_IN_FLIGHT);
		for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame) {
			for (size_t binding = 0; binding < descriptor_bindings.size(); ++binding) {
				if (descriptor_bindings[binding].descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					continue;

				uint32_t binding_frame_offset = frame * descriptor_bindings.size();
				VK_BufferInfo buffer_info = {};

				auto binding_memory_type_it = binding_memory_type_map.find(binding);
				if (binding_memory_type_it != binding_memory_type_map.end()) {
					buffer_info.preferred_memory_type_flag = binding_memory_type_it->second.preferred_memory_type;
				} else {
					buffer_info.preferred_memory_type_flag = info.preferred_memory_type_flags;
				}


				buffer_info.size = pipeline_layout->getBindingElementSize(binding);
				buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

				uniform_buffers[binding_frame_offset + binding].alloc(context, buffer_info);
			}
		}
		createDescriptorWrites();

		//allocate descriptor sets
		set_indices.resize(pipeline_layout->getDescriptorSetCount() * MAX_FRAMES_IN_FLIGHT);
		descriptor_set_handles.resize(pipeline_layout->getDescriptorSetCount() * MAX_FRAMES_IN_FLIGHT);
		descriptor_allocations.resize(pipeline_layout->getDescriptorSetCount() * MAX_FRAMES_IN_FLIGHT);
		uint32_t set_count = pipeline_layout->getDescriptorSetCount() * MAX_FRAMES_IN_FLIGHT;
		for (int i = 0; i < set_count * MAX_FRAMES_IN_FLIGHT; ++i) {
			uint32_t set_layout_index = i % pipeline_layout->getDescriptorSetCount();
			const VK_DescriptorSetLayout &set_layout = pipeline_layout->getDescriptorSetLayouts()[set_layout_index];
			set_indices[i] = set_layout.getDescriptorSetIndex();
			descriptor_set_handles[i] = VK_NULL_HANDLE;
			descriptor_allocations[i] = {};
		}
		context->descriptor_allocator.alloc(pipeline_layout, set_indices.data(), descriptor_allocations.data(), set_count* MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < set_count * MAX_FRAMES_IN_FLIGHT; ++i) {
			descriptor_set_handles[i] = context->descriptor_allocator.getDescriptorSet(descriptor_allocations[i]);
		}
		context->renderer.onFrameEnd.subscribe(on_frame_change_subscription_id, this, &VK_PipelineInstance::onFrameEnd);
	}

	void VK_PipelineInstance::release() {
		context->renderer.onFrameEnd.unsubscribe(on_frame_change_subscription_id);

		for (VK_Buffer &b: uniform_buffers) {
			if (b.allocated())
				b.release();
		}
		for (VkDescriptorImageInfo *image_info: image_infos) {
			delete[] image_info;
		}
		for (VkDescriptorBufferInfo *buffer_info: buffer_infos) {
			delete[] buffer_info;
		}
		for (VkBufferView *views: buffer_views) {
			delete[] views;
		}
		for (DescriptorSetAllocation &descriptor_allocation: descriptor_allocations) {
			context->descriptor_allocator.free(descriptor_allocation);
		}

		pipeline_layout = nullptr;
	}

	void VK_PipelineInstance::createDescriptorWrites() {
		const std::vector<VkDescriptorSetLayoutBinding> layout_bindings = pipeline_layout->getDescriptorBindings();
		const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts = pipeline_layout->getDescriptorSetLayoutHandles();
		const std::vector<VK_BindingInfo> &descriptor_infos = pipeline_layout->getBindingInfos();

		writes.resize(layout_bindings.size());
		dirty_descriptor_sets_bindings.resize(MAX_FRAMES_IN_FLIGHT * layout_bindings.size(), true);
		uint32_t frame_index = 0;
		for (size_t binding = 0; binding < layout_bindings.size(); ++binding) {
			auto descriptor_type = layout_bindings[binding].descriptorType;
			uint32_t descriptor_count = layout_bindings[binding].descriptorCount;
			uint32_t descriptor_set_index = (frame_index * descriptor_set_layouts.size()) + descriptor_infos[binding].descriptor_set_index;

			if (pipeline_layout->IsBindingVariableSize(binding)) {
				descriptor_count = 0;
			}

			VkWriteDescriptorSet write = {};
			write.descriptorType = descriptor_type;
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = binding;
			write.descriptorType = descriptor_type;
			write.dstArrayElement = 0;
			write.descriptorCount = descriptor_count;
			write.pTexelBufferView = nullptr; // Optional

			writes[binding] = write;
		}

		buffer_infos.resize(layout_bindings.size() * MAX_FRAMES_IN_FLIGHT, nullptr);
		image_infos.resize(layout_bindings.size() * MAX_FRAMES_IN_FLIGHT, nullptr);
		buffer_views.resize(layout_bindings.size() * MAX_FRAMES_IN_FLIGHT, nullptr);
		acceleration_structure_infos.resize(layout_bindings.size() * MAX_FRAMES_IN_FLIGHT, {});
		for (int binding = 0; binding < writes.size(); ++binding) {
			//set uniform buffers
			if (writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				buffer_infos[binding] = new VkDescriptorBufferInfo[writes[binding].descriptorCount];
				buffer_infos[binding]->buffer = uniform_buffers[binding].getVkHandle();
				buffer_infos[binding]->offset = 0;
				buffer_infos[binding]->range = uniform_buffers[binding].getSize();
				writes[binding].pBufferInfo = buffer_infos[binding];
			}
			if (writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
			    writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
			    writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
				if (image_infos[binding] != nullptr)
					delete image_infos[binding];
				image_infos[binding] = new VkDescriptorImageInfo[writes[binding].descriptorCount];
				writes[binding].pImageInfo = image_infos[binding];
			}
			if (writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
				if (buffer_infos[binding] != nullptr)
					delete buffer_infos[binding];
				buffer_infos[binding] = new VkDescriptorBufferInfo[writes[binding].descriptorCount];
				for (uint32_t i = 0; i < writes[binding].descriptorCount; ++i) {
					buffer_infos[binding][i].buffer = VK_NULL_HANDLE;
					buffer_infos[binding][i].offset = 0;
					buffer_infos[binding][i].range = 0;
				}
				writes[binding].pBufferInfo = buffer_infos[binding];
			}
			if (writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
				if (buffer_infos[binding] != nullptr)
					delete buffer_infos[binding];
				if (buffer_views[binding] != nullptr)
					delete buffer_views[binding];
				buffer_infos[binding] = new VkDescriptorBufferInfo[writes[binding].descriptorCount];
				buffer_views[binding] = new VkBufferView[writes[binding].descriptorCount];
				for (uint32_t i = 0; i < writes[binding].descriptorCount; ++i) {
					buffer_infos[binding][i].buffer = VK_NULL_HANDLE;
					buffer_infos[binding][i].offset = 0;
					buffer_infos[binding][i].range = 0;
					buffer_views[binding][i] = VK_NULL_HANDLE;
				}
				writes[binding].pBufferInfo = buffer_infos[binding];
			}
			if (writes[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				acceleration_structure_infos[binding] = {};
				acceleration_structure_infos[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			}
		}
	}


	void VK_PipelineInstance::updateDescriptors() {
		HB_PROFILE_BEGIN("UPDATE_DESCRIPTORS");
		uint32_t binding_count = pipeline_layout->getDescriptorBindings().size();
		uint32_t frame = context->renderer.getCurrentFrameIndex();
		uint32_t frame_binding_offset = frame * binding_count;
		std::vector<VkWriteDescriptorSet> out_of_date_descriptor_writes;
		//fix dirty bindings, update with new writes.
		for (uint32_t binding = 0; binding < binding_count; ++binding) {
			if (!dirty_descriptor_sets_bindings[frame_binding_offset + binding])
				continue;
			dirty_descriptor_sets_bindings[frame_binding_offset + binding] = false;
			writes[binding].dstSet = getDescriptorSetForBinding(binding);
			if (writes[binding].descriptorCount != 0)
				out_of_date_descriptor_writes.emplace_back(writes[binding]);
			else
				Log::debug("Descriptor set for binding " + std::to_string(binding) + " is empty, skipping update");
		}
		if (!out_of_date_descriptor_writes.empty()) {
			vkUpdateDescriptorSets(context->device.getHandle(), out_of_date_descriptor_writes.size(), out_of_date_descriptor_writes.data(), 0, nullptr);
		}

		HB_PROFILE_END("UPDATE_DESCRIPTORS");
	}

	void VK_PipelineInstance::bind() {
		if (bound) return;

		updateDescriptors();

		uint32_t frame = context->renderer.getCurrentFrameIndex();
		uint32_t descriptor_set_count = pipeline_layout->getDescriptorSetCount();
		uint32_t descriptor_frame_offset = frame * descriptor_set_count;
		VkCommandBuffer command_buffer = context->renderer.getCommandPool()->getCurrentBuffer();

		Log::debug("Binding pipeline instance with " + std::to_string(descriptor_set_count) + " descriptor sets for frame " + std::to_string(frame));
		vkCmdBindDescriptorSets(command_buffer,
		                        pipeline_layout->getBindPoint(),
		                        pipeline_layout->getHandle(),
		                        0,
		                        descriptor_set_count,
		                        descriptor_set_handles.data() + descriptor_frame_offset,
		                        0,
		                        nullptr);
		bound = true;
	}

	void VK_PipelineInstance::getBinding(const char *name, uint32_t &binding) {
		binding = getBinding(name);
	}

	void VK_PipelineInstance::bind(VkCommandBuffer command_buffer, uint32_t frame) {
		if (bound) return;
		updateDescriptors();


		uint32_t descriptor_set_count = pipeline_layout->getDescriptorSetLayoutHandles().size();
		uint32_t offset = descriptor_set_count * frame;
		vkCmdBindDescriptorSets(command_buffer,
		                        pipeline_layout->getBindPoint(),
		                        pipeline_layout->getHandle(),
		                        0,
		                        descriptor_set_count,
		                        descriptor_set_handles.data() + offset,
		                        0,
		                        nullptr);
		bound = true;
	}

	void VK_PipelineInstance::unbind() {
		bound = false;
	}

	uint32_t VK_PipelineInstance::getBinding(const char *name) const {
		return pipeline_layout->getDescriptorBinding(name);
	}

	uint32_t VK_PipelineInstance::getBindingIndexForFrame(uint32_t binding) {
		uint32_t frame = context->renderer.getCurrentFrameIndex();
		uint32_t binding_count = pipeline_layout->getDescriptorBindings().size();
		uint32_t binding_frame_offset = binding_count * frame;
		return binding_frame_offset + binding;
	}

	VkDescriptorSet VK_PipelineInstance::getDescriptorSetForBinding(uint32_t binding) {
		uint32_t frame = context->renderer.getCurrentFrameIndex();
		uint32_t descriptor_set_count = pipeline_layout->getDescriptorSetCount();
		uint32_t descriptor_frame_offset = frame * descriptor_set_count;
		uint32_t descriptor_index = pipeline_layout->getBindingInfos()[binding].descriptor_set_index;
		return context->descriptor_allocator.getDescriptorSet(descriptor_allocations[descriptor_index + descriptor_frame_offset]);
	}

	void VK_PipelineInstance::setImageArray(uint32_t binding, ImageHandle *textures, uint32_t texture_count, int32_t mip_level) {
		const VkDescriptorSetLayoutBinding &layout_binding = pipeline_layout->getDescriptorBindings()[binding];
		const VK_BindingInfo &binding_info = pipeline_layout->getBindingInfos()[binding];
		uint32_t set_index = binding_info.descriptor_set_index;
		uint32_t max_descriptor_count = layout_binding.descriptorCount;
		bool variable_size = pipeline_layout->IsBindingVariableSize(binding);
		HB_ASSERT(layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
		          layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
		          layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, "binding#" + std::to_string(binding) + " is not a texture");
		HB_ASSERT(variable_size || (texture_count <= max_descriptor_count),
		          "descriptor count mismatch for binding#" + std::to_string(binding) + " (texture_count: " + std::to_string(texture_count) + " > descriptorCount: " +
		          std::to_string(max_descriptor_count) + ")");

		VkWriteDescriptorSet &write_descriptor_set = writes[binding];

		//todo: we need to allocate more descriptor sets
		if (variable_size && texture_count > max_descriptor_count) {
			//todo: resize descriptors we are over the descriptor count allocated for this bindingtexture_count = max_descriptor_count;
			DescriptorSetAllocation allocation{};
			//todo: realloc new descriptor with new count, need a way to precise what count we want in the variable set.
			context->descriptor_allocator.alloc(pipeline_layout, &set_index, &allocation, 1, &texture_count);
			context->descriptor_allocator.free(descriptor_allocations[binding_info.descriptor_set_index]);
			descriptor_allocations[binding_info.descriptor_set_index] = allocation;
			for (int i = 0; i < descriptor_set_handles.size(); ++i) {
				descriptor_set_handles[i] = context->descriptor_allocator.getDescriptorSet(descriptor_allocations[i]);
			}

		}

		//we need to allocate more image_infos
		if (texture_count > write_descriptor_set.descriptorCount) {
			if (image_infos[binding] != nullptr)
				delete image_infos[binding];
			image_infos[binding] = new VkDescriptorImageInfo[texture_count];
		}

		write_descriptor_set.descriptorCount = texture_count;
		for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; ++i) {
			int index = i >= texture_count ? texture_count - 1 : i;
			VK_Image &vk_image = context->images[textures[index]];
			image_infos[binding][i].imageLayout = vk_image.getImageLayout();
			image_infos[binding][i].imageView = vk_image.getImageView(mip_level);
			image_infos[binding][i].sampler = vk_image.getSampler();
		}

		writes[binding].pImageInfo = image_infos[binding];
		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setImage(uint32_t binding, ImageHandle image, uint32_t mip_level) {
		VK_Image &vk_image_object = context->images[image];
		HB_ASSERT(writes[binding].descriptorCount == 1, "Texture binding is an array");

		writes[binding].descriptorCount = 1;

		VkDescriptorImageInfo &image_info = *image_infos[binding];
		image_info.imageView = vk_image_object.getImageView(mip_level);
		image_info.sampler = vk_image_object.getSampler();
		image_info.imageLayout = vk_image_object.getImageLayout();

		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setUniform(uint32_t binding, const void *data) {
		HB_ASSERT(pipeline_layout->getDescriptorBindings()[binding].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		          "binding#" + std::to_string(binding) + " is not a uniform buffer");
		uint32_t binding_frame_index = getBindingIndexForFrame(binding);
		uniform_buffers[binding_frame_index].update(data);
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			buffer_infos[binding]->buffer = uniform_buffers[binding_frame_index].getVkHandle();
		}
		//no need to do this, it is already done in the createDescriptorWrites function
		//uniform_buffer_infos[write_index].buffer = uniform_buffers[write_index].getHandle();

		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setAccelerationStructure(uint32_t binding, RootAccelerationStructureHandle acceleration_structure) {
		VK_TopLevelAccelerationStructure &vk_top_level_acceleration_structure_object = context->root_acceleration_structures[acceleration_structure];

		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
		const VkAccelerationStructureKHR &acceleration_structure_handle = vk_top_level_acceleration_structure_object.getVkHandle();

		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
		          "binding#" + std::to_string(binding) + " is not an acceleration structure");

		VkWriteDescriptorSetAccelerationStructureKHR &accelerationStructureInfo = acceleration_structure_infos[binding];
		accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		accelerationStructureInfo.accelerationStructureCount = 1;
		accelerationStructureInfo.pAccelerationStructures = &acceleration_structure_handle;

		writes[binding].pNext = &acceleration_structure_infos[binding];

		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setStorageBufferArray(uint32_t binding, BufferHandle *buffers, uint32_t count) {
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");
		HB_ASSERT(pipeline_layout->IsBindingVariableSize(binding) || (count <= descriptorSetLayoutBinding.descriptorCount), "descriptor count mismatch");

		if (pipeline_layout->IsBindingVariableSize(binding) && count > writes[binding].descriptorCount) {
			writes[binding].descriptorCount = count;
			if (image_infos[binding] != nullptr)
				delete image_infos[binding];
			buffer_infos[binding] = new VkDescriptorBufferInfo[count];
		}

		for (int i = 0; i < count; i++) {
			HB_ASSERT(context->buffers.valid(buffers[i]), "Buffer " + std::to_string(i) + " handle is invalid");
			VK_Buffer &vk_buffer = context->buffers[buffers[i]];
			buffer_infos[binding][i].buffer = vk_buffer.getVkHandle();
			buffer_infos[binding][i].offset = 0;
			buffer_infos[binding][i].range = vk_buffer.getSize();
		}

		writes[binding].pBufferInfo = buffer_infos[binding];
		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setBindingDirty(uint32_t binding) {
		for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame) {
			uint32_t binding_count = pipeline_layout->getDescriptorBindings().size();
			uint32_t frame_binding_offset = frame * binding_count;
			dirty_descriptor_sets_bindings[frame_binding_offset + binding] = true;
		}
	}

	void VK_PipelineInstance::setStorageBuffer(uint32_t binding, BufferHandle buffer, size_t byte_offset) {
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");

		uint32_t write_index = getBindingIndexForFrame(binding);
		VK_Buffer &vk_buffer = context->buffers[buffer];

		buffer_infos[write_index]->buffer = vk_buffer.getVkHandle();
		buffer_infos[write_index]->offset = byte_offset;
		buffer_infos[write_index]->range = vk_buffer.getSize();

		writes[write_index].pBufferInfo = buffer_infos[write_index];

		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setTexelBuffer(uint32_t binding, TexelBufferHandle buffer) {
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");

		VK_TexelBuffer &vk_texel_buffer = context->texel_buffers[buffer];

		writes[binding].pTexelBufferView = &vk_texel_buffer.getView();

		setBindingDirty(binding);
	}

	void VK_PipelineInstance::setTexelBufferArray(uint32_t binding, TexelBufferHandle *buffers, uint32_t buffer_count) {
		const VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding = pipeline_layout->getDescriptorBindings()[binding];
		HB_ASSERT(descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "binding#" + std::to_string(binding) + " is not a storage buffer");
		HB_ASSERT(buffer_count <= descriptorSetLayoutBinding.descriptorCount || descriptorSetLayoutBinding.descriptorCount == 0, "descriptor count mismatch");


		if (pipeline_layout->IsBindingVariableSize(binding) && buffer_count > writes[binding].descriptorCount) {
			writes[binding].descriptorCount = buffer_count;
			if (buffer_infos[binding] != nullptr)
				delete buffer_infos[binding];
			if (buffer_views[binding] != nullptr)
				delete buffer_views[binding];
			buffer_infos[binding] = new VkDescriptorBufferInfo[buffer_count];
			buffer_views[binding] = new VkBufferView[buffer_count];
		}

		for (int i = 0; i < buffer_count; i++) {
			HB_ASSERT(context->texel_buffers.valid(buffers[i]), "Texel buffer " + std::to_string(i) + " handle is invalid");
			VK_TexelBuffer &vk_buffer = context->texel_buffers[buffers[i]];
			buffer_views[binding][i] = vk_buffer.getView();
			buffer_infos[binding][i].buffer = vk_buffer.getHandle();
			buffer_infos[binding][i].range = vk_buffer.getSize();
		}

		writes[binding].pTexelBufferView = buffer_views[binding];
		writes[binding].pBufferInfo = buffer_infos[binding];
		setBindingDirty(binding);
	}

	void VK_PipelineInstance::onFrameEnd(uint32_t frame) {
		while (!old_descriptor_pools.empty() && old_descriptor_pools.front().first == frame) {
			vkDestroyDescriptorPool(context->device.getHandle(), old_descriptor_pools.front().second, nullptr);
			old_descriptor_pools.pop();
		}
	}

	Handle VK_PipelineInstance::getPipeline() {
		return pipeline_handle;
	}

	bool VK_PipelineInstance::allocated() {
		return pipeline_layout != nullptr;
	}
}
