
#include "VK_Allocator.h"
#include "VK_CommandPool.h"

#include "VK_Device.h"
#include "core/utility/Log.h"

namespace HBE {
	VkMemoryPropertyFlags choseProperties(AllocFlags flags) {
		VkMemoryPropertyFlags properties = 0;
		if (flags & MAPPABLE) {
			properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		} else {
			properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		return properties;
	}

	VK_Allocator::VK_Allocator(const VK_Device *device) {
		this->device = device;
		this->command_pool = new VK_CommandPool(device, 1);
		memory_propeties=&device->getPhysicalDevice().getMemoryProperties();
		for (int i = 0; i < memory_propeties->memoryTypeCount; ++i) {
			blocks.emplace(i,std::vector<Block>());
		}
	}

	VK_Allocator::~VK_Allocator() {
		delete command_pool;
		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			for (Block block:it->second) {
				vkFreeMemory(device->getHandle(), block.memory, nullptr);
			}
		}
	}

	uint32_t VK_Allocator::findMemoryType(VkMemoryRequirements memory_requirement,AllocFlags flags) {
		//todo:handle out of memory.
		// if all heap of a memory type are out of memory, return another memory type.
		VkMemoryPropertyFlags properties = choseProperties(flags);
		uint32_t type_filter=memory_requirement.memoryTypeBits;
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

		return std::to_string(alloc.size) + " bytes of " + memoryTypeToString(alloc.block.memory_type) + " at position " + std::to_string(alloc.offset) + " in block " + std::to_string(alloc.block.index);

	}

	//https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
	Allocation &VK_Allocator::alloc(VkMemoryRequirements mem_requirements, AllocFlags flags) {
		uint32_t memory_type = findMemoryType(mem_requirements,flags);
		for (auto block_it = blocks[memory_type].begin(); block_it != blocks[memory_type].end(); ++block_it) {
			if (block_it->remaining < mem_requirements.size) {
				continue;
			}
			VkDeviceSize position = 0;
			for (auto allocation_it = block_it->allocations.begin(); allocation_it != block_it->allocations.end(); ++allocation_it) {
				VkDeviceSize end = allocation_it->offset - (allocation_it->offset % mem_requirements.alignment);
				VkDeviceSize available = (end - position);

				if (available >= mem_requirements.size) {
					block_it->alloc_count++;
					block_it->remaining -= mem_requirements.size;
					std::list<Allocation>::iterator new_alloc_it = block_it->allocations.emplace(allocation_it, Allocation{
							.size=mem_requirements.size,
							.offset=position,
							.block=*block_it,
							.flags=flags});
					new_alloc_it->position_in_block = new_alloc_it;
					Log::debug("Allocated " + allocToString(*new_alloc_it));
					return *new_alloc_it;

				} else {
					position = allocation_it->offset + allocation_it->size;
					auto adjustement = position % mem_requirements.alignment;
					if (adjustement != 0) {
						position += mem_requirements.alignment - adjustement;
					}

				}
			}
			VkDeviceSize end = block_it->size - (block_it->size % mem_requirements.alignment);
			if (end - position >= mem_requirements.size) {
				block_it->alloc_count++;

				block_it->remaining -= mem_requirements.size;
				const std::list<Allocation>::iterator &new_alloc_it = block_it->allocations.emplace(block_it->allocations.end(),
																									Allocation{
																											.size=mem_requirements.size,
																											.offset=position,
																											.block=*block_it,
																											.flags=flags});
				new_alloc_it->position_in_block = new_alloc_it;

				Log::debug("Allocated " + allocToString(*new_alloc_it));
				return *new_alloc_it;
			}
		}
		//new block needed

		uint32_t index = blocks[memory_type].size();
		Block &block = blocks[memory_type].emplace_back(Block{
				.size= mem_requirements.size > BLOCK_SIZE ? mem_requirements.size : BLOCK_SIZE,
				.allocations=std::list<Allocation>(),
				.memory=VK_NULL_HANDLE,
				.memory_type=memory_type,
				.index=index,
				.alloc_count=0,
				.remaining=(mem_requirements.size > BLOCK_SIZE ? mem_requirements.size : BLOCK_SIZE) - mem_requirements.size});


		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = block.size;
		allocInfo.memoryTypeIndex = memory_type;

		if (vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &block.memory) != VK_SUCCESS) {
			Log::error("failed to allocate buffer memory!");
		}
		block.alloc_count++;
		auto new_alloc_it = block.allocations.emplace(block.allocations.end(), Allocation{
				.size=mem_requirements.size,
				.offset=0,
				.block=block,
				.flags=flags});
		new_alloc_it->position_in_block = new_alloc_it;
		Log::debug("Created block " + std::to_string(block.index) + " of type " + memoryTypeToString(block.memory_type) + " with " + std::to_string(block.size / 1048576.0f) + "mb");
		Log::debug("Allocated " + allocToString(*new_alloc_it));
		return *new_alloc_it;
	}

	void VK_Allocator::copy(VkBuffer src, VkBuffer dest, VkDeviceSize size) {
		command_pool->begin(0);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // buffer offset not memory
		copyRegion.dstOffset = 0; // buffer offset not memory
		copyRegion.size = size;
		vkCmdCopyBuffer(command_pool->getCurrentBuffer(), src, dest, 1, &copyRegion);

		command_pool->end(0);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_pool->getCurrentBuffer();

		//todo use transfer queue
		vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->getGraphicsQueue());
	}

	void VK_Allocator::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
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
				0, nullptr,
				0, nullptr,
				1, &barrier
		);
	}

	void VK_Allocator::copy(VkBuffer src, VkImage dest, uint32_t width, uint32_t height, uint32_t depth) {

		command_pool->begin(0);
		transitionImageLayout(dest, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
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
				width,
				height,
				depth
		};
		vkCmdCopyBufferToImage(
				command_pool->getCurrentBuffer(),
				src,
				dest,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
		);

		transitionImageLayout(dest, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		command_pool->end(0);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_pool->getCurrentBuffer();

		//todo use transfer queue
		vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->getGraphicsQueue());
	}

	void VK_Allocator::free(Allocation &allocation) {
		allocation.block.alloc_count--;
		uint32_t memory_type = allocation.block.memory_type;
		Log::debug("Freed " + allocToString(allocation));
		if (allocation.block.alloc_count == 0) {
			vkFreeMemory(device->getHandle(), allocation.block.memory, nullptr);
			Log::debug("Freed block " + std::to_string(allocation.block.index) + " of " + memoryTypeToString(allocation.block.memory_type));
			auto it = blocks[memory_type].erase(blocks[memory_type].begin() + allocation.block.index);

			for (; it != blocks[memory_type].end(); ++it) {
				it->index--;
			}
		} else {
			allocation.block.allocations.erase(allocation.position_in_block);
		}


	}

}
