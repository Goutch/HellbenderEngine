
#include "VK_Allocator.h"
#include "VK_CommandPool.h"
#include "VK_Image.h"
#include "VK_Semaphore.h"

namespace HBE {
	VkMemoryPropertyFlags choseProperties(ALLOC_FLAGS flags) {
		VkMemoryPropertyFlags properties = 0;
		if (flags & ALLOC_FLAG_MAPPABLE) {
			properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		} else {
			properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		return properties;
	}

	VK_Allocator::VK_Allocator(VK_Device *device) {
		this->device = device;
		this->command_pool = new VK_CommandPool(device, 1);
		memory_propeties = &device->getPhysicalDevice().getMemoryProperties();
		for (size_t i = 0; i < memory_propeties->memoryTypeCount; ++i) {
			blocks.emplace(i, std::vector<Block *>());
		}
	}

	VK_Allocator::~VK_Allocator() {
		delete command_pool;
		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			for (Block *block: it->second) {
				vkFreeMemory(device->getHandle(), block->memory, nullptr);
			}
		}
	}

	uint32_t VK_Allocator::findMemoryType(VkMemoryRequirements memory_requirement, ALLOC_FLAGS flags) {
		//todo: handle out of memory.
		//if all heap of a memory type are out of memory, return another memory type.
		VkMemoryPropertyFlags properties = choseProperties(flags);
		uint32_t type_filter = memory_requirement.memoryTypeBits;

		for (uint32_t i = 0; i < memory_propeties->memoryTypeCount; i++) {
			if ((type_filter & (1 << i)) &&
				(memory_propeties->memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		Log::error("Failed to find suitable memory type");
		return 0;
	}

	std::string VK_Allocator::memoryTypeToString(const uint32_t mem_type) {

		VkMemoryPropertyFlags flags = device->getPhysicalDevice().getMemoryProperties().memoryTypes[mem_type].propertyFlags;
		std::string type = "";
		bool need_separator = false;
		std::string separator = " & ";
		if (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT & flags) {
			type += "\"Device local\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & flags) {
			if (need_separator)
				type += separator;
			type += "\"Host visible\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_CACHED_BIT & flags) {
			if (need_separator)
				type += separator;
			type += "\"Host chached\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & flags) {
			if (need_separator)
				type += separator;
			type += "\"Host coherent\"";
		}
		return type;
	}

	std::string VK_Allocator::allocToString(const Allocation &alloc) {

		return "Alloc#" + std::to_string(alloc.id) + " " + std::to_string(alloc.size) + " bytes of " + memoryTypeToString(alloc.block->memory_type) +
			   " at position " + std::to_string(alloc.offset) + " in block " + std::to_string(alloc.block->index);

	}

	//https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
	Allocation VK_Allocator::alloc(VkMemoryRequirements mem_requirements, ALLOC_FLAGS flags) {
		uint32_t memory_type = findMemoryType(mem_requirements, flags);

		for (Block *block:blocks[memory_type]) {
			if (block->remaining < mem_requirements.size) {
				continue;
			}
			VkDeviceSize position = 0;

			for (const Allocation &current_alloc:block->allocations) {
				int32_t end_of_new_alloc = current_alloc.offset - (current_alloc.offset % mem_requirements.alignment);
				int32_t available = end_of_new_alloc - position;

				if (available >= static_cast<int32_t>(mem_requirements.size)) {
					block->alloc_count++;
					block->remaining -= mem_requirements.size;
					const Allocation &new_alloc = *block->allocations.emplace(
							mem_requirements.size,
							position,
							block,
							flags,
							current_id++).first;
					//Log::debug("Allocated " + allocToString(new_alloc));
					return new_alloc;

				} else {
					position = current_alloc.offset + current_alloc.size;
					VkDeviceSize adjustement = position % mem_requirements.alignment;
					if (adjustement != 0) {
						position += mem_requirements.alignment - adjustement;
					}

				}
			}
			VkDeviceSize end_of_alloc = block->size - (block->size % mem_requirements.alignment);
			if (end_of_alloc - position >= mem_requirements.size) {
				block->alloc_count++;

				block->remaining -= mem_requirements.size;
				const Allocation &new_alloc = *block->allocations.emplace(
						mem_requirements.size,
						position,
						block,
						flags,
						current_id++).first;

				//Log::debug("Allocated " + allocToString(new_alloc));
				return new_alloc;
			}
		}
		//new block needed

		uint32_t index = blocks[memory_type].size();
		Block *block = blocks[memory_type].emplace_back(new Block{
				.size= mem_requirements.size > BLOCK_SIZE ? mem_requirements.size : BLOCK_SIZE,
				.memory=VK_NULL_HANDLE,
				.memory_type=memory_type,
				.index=index,
				.alloc_count=0,
				.remaining=(mem_requirements.size > BLOCK_SIZE ? mem_requirements.size : BLOCK_SIZE) - mem_requirements.size});


		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = block->size;
		allocInfo.memoryTypeIndex = memory_type;

		if (vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &block->memory) != VK_SUCCESS) {
			Log::error("failed to allocate buffer memory!");
		}
		block->alloc_count++;
		const Allocation &new_alloc = *block->allocations.emplace(
				mem_requirements.size,
				VkDeviceSize(0),
				block,
				flags,
				current_id++).first;
		/*Log::debug(
				"Created block " + std::to_string(block->index) + " of type " + memoryTypeToString(block->memory_type) +
				" with " + std::to_string(block->size / 1048576.0f) + "mb");
		Log::debug("Allocated " + allocToString(new_alloc));*/
		return new_alloc;
	}

	void VK_Allocator::copy(VkBuffer src, VkBuffer dest, VkDeviceSize size) {
		command_pool->begin(0);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // buffer offset not memory
		copyRegion.dstOffset = 0; // buffer offset not memory
		copyRegion.size = size;
		vkCmdCopyBuffer(command_pool->getCurrentBuffer(), src, dest, 1, &copyRegion);

		command_pool->end(0);


		//todo use transfer queue
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->submit(command_pool->getCurrentBuffer());
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
		//command_pool->reset(0);
	}

	void VK_Allocator::transitionImageLayout(VK_Image *image, VkImageLayout new_layout) {
		VkImageLayout old_layout = image->getImageLayout();
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image->getHandle();
		barrier.subresourceRange.aspectMask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ?
											  VK_IMAGE_ASPECT_DEPTH_BIT :
											  VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			//todo: stencil
			/*if (hasStencilComponent(format)) {
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}*/
		} else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;


		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_ACCESS_TRANSFER_WRITE_BIT;
		} else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
				   new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			Log::error("unsupported layout transition!");
			return;
		}
		vkCmdPipelineBarrier(
				command_pool->getCurrentBuffer(),
				sourceStage, destinationStage,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier
		);

		image->setImageLayout(new_layout);

	}

	void VK_Allocator::copy(VkBuffer src, VK_Image *dest, VkImageLayout dst_end_layout) {
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
		command_pool->begin(0);
		transitionImageLayout(dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
				dest->getWidth(),
				dest->getHeight(),
				dest->getDepth()
		};
		vkCmdCopyBufferToImage(
				command_pool->getCurrentBuffer(),
				src,
				dest->getHandle(),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
		);

		transitionImageLayout(dest, dst_end_layout);
		command_pool->end(0);


		device->getQueue(QUEUE_FAMILY_GRAPHICS)->submit(command_pool->getCurrentBuffer());
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
		//command_pool->reset(0);
	}

	void VK_Allocator::copy(VK_Image *src, VkImageLayout src_end_layout, VK_Image *dest, VkImageLayout dst_end_layout) {


		VkImageAspectFlagBits src_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;//todo change for depth if image is depth or stensil
		VkImageAspectFlagBits dst_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

		command_pool->begin(0);
		transitionImageLayout(dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		transitionImageLayout(src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		VkImageCopy region{};
		region.dstOffset = {0, 0, 0};
		region.extent = {src->getWidth(), src->getHeight(), src->getDepth()};
		region.srcOffset = {0, 0, 0};
		region.dstSubresource.aspectMask = dst_aspect_mask;
		region.dstSubresource.mipLevel = 0;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.layerCount = 0;
		region.srcSubresource.aspectMask = src_aspect_mask;
		region.srcSubresource.mipLevel = 0;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.layerCount = 0;
		vkCmdCopyImage(
				command_pool->getCurrentBuffer(),
				src->getHandle(),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dest->getHandle(),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
		);
		transitionImageLayout(dest, dst_end_layout);
		transitionImageLayout(src, src_end_layout);
		command_pool->end(0);

		device->getQueue(QUEUE_FAMILY_GRAPHICS)->submit(command_pool->getCurrentBuffer());
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
	}

	void VK_Allocator::free(const Allocation &allocation) {
		//Log::debug("Freeing Alloc#" + std::to_string(allocation.id));

		Block *block = allocation.block;
		block->alloc_count--;
		uint32_t memory_type = allocation.block->memory_type;
		//Log::debug("Freed " + allocToString(allocation));
		if (allocation.block->alloc_count == 0) {
			vkFreeMemory(device->getHandle(), allocation.block->memory, nullptr);
			//Log::debug("Freed block " + std::to_string(allocation.block->index) + " of " +
			// memoryTypeToString(allocation.block->memory_type));
			auto it = blocks[memory_type].erase(blocks[memory_type].begin() + allocation.block->index);
			delete block;
			for (; it != blocks[memory_type].end(); ++it) {
				(*it)->index--;
			}
		} else {
			allocation.block->allocations.erase(allocation);
		}
	}


	void VK_Allocator::setImageLayout(VK_Image *image, VkImageLayout newLayout) {
		command_pool->begin(0);
		transitionImageLayout(image, newLayout);
		command_pool->end(0);

		device->getQueue(QUEUE_FAMILY_GRAPHICS)->submit(command_pool->getCurrentBuffer());
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
	}
}

