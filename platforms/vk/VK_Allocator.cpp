#include <algorithm>
#include "VK_Allocator.h"
#include "platforms/vk/VK_CommandPool.h"
#include "platforms/vk/VK_Image.h"
#include "platforms/vk/VK_Buffer.h"
#include "cstring"
#include "VK_Renderer.h"
#include "Application.h"
#include "VK_Utils.h"
#include "core/graphics/Graphics.h"

namespace HBE
{
	VkMemoryPropertyFlags choseProperties(MEMORY_TYPE_FLAGS flags)
	{
		VkMemoryPropertyFlags properties = 0;
		if (flags & MEMORY_TYPE_FLAG_MAPPABLE)
		{
			properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		if (flags & MEMORY_TYPE_FLAG_GPU_LOCAL)
		{
			properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		return properties;
	}

	VK_Allocator::VK_Allocator(VK_Device* device)
	{
		this->device = device;
		this->command_pool = new VK_CommandPool(*device, 1, device->getQueue(QUEUE_FAMILY_GRAPHICS));

		Graphics::onFrameChange.subscribe(this, &VK_Allocator::freeStagingBuffers);
		memory_properties = &device->getPhysicalDevice().getMemoryProperties();
		for (int i = 0; i < memory_properties->memoryHeapCount; ++i)
		{
			memory_heaps.push_back({
				memory_properties->memoryHeaps[i].size, 0,
				false
			});
			memory_heaps[i].block_size = glm::min(static_cast<VkDeviceSize>(std::floor(memory_heaps[i].max_size / 10)),
			                                      BLOCK_SIZE);
		}
		for (uint32_t i = 0; i < memory_properties->memoryTypeCount; ++i)
		{
			memory_types.push_back({
				i,
				memory_properties->memoryTypes[i].heapIndex,
				(uint32_t(1) << i),
				memory_properties->memoryTypes[i].propertyFlags
			});
			memory_heaps[memory_properties->memoryTypes[i].heapIndex].memory_properties_flags |= memory_properties->
				memoryTypes[i].propertyFlags;
		}

		for (int i = 0; i < memory_heaps.size(); ++i)
		{
			std::vector<std::string> properties;
			if (memory_heaps[i].memory_properties_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				properties.emplace_back("DEVICE_LOCAL");
			}
			if (memory_heaps[i].memory_properties_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				properties.emplace_back("HOST_VISIBLE");
			}
			if (memory_heaps[i].memory_properties_flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
			{
				properties.emplace_back("HOST_CACHED");
			}
			if (memory_heaps[i].memory_properties_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			{
				properties.emplace_back("HOST_COHERENT");
			}
			std::string properties_str;
			for (int j = 0; j < properties.size(); ++j)
			{
				properties_str += properties[j];
				if (j < properties.size() - 1)
				{
					properties_str += " | ";
				}
			}

			Log::message(
				"Memory heap " + std::to_string(i) + " of " + std::to_string(memory_heaps[i].max_size / (1024 * 1024)) +
				"mb and properties: " + properties_str);
		}

		for (size_t i = 0; i < memory_properties->memoryHeapCount; ++i)
		{
			blocks.emplace(i, std::vector<Block*>());
		}
	}


	VK_Allocator::~VK_Allocator()
	{
		freeStagingBuffers(0);
		delete command_pool;
		for (auto& it : blocks)
		{
			for (Block* block : it.second)
			{
				vkFreeMemory(device->getHandle(), block->memory, nullptr);
			}
		}
		for (auto pooled_block : block_cache)
		{
			vkFreeMemory(device->getHandle(), pooled_block.second->memory, nullptr);
			delete pooled_block.second;
		}
		Graphics::onFrameChange.unsubscribe(this);
	}

	uint32_t VK_Allocator::findMemoryTypeIndex(VkMemoryRequirements memory_requirement, MEMORY_TYPE_FLAGS flags)
	{
		//if all heap of a memory type are out of memory, return another memory type.
		VkMemoryPropertyFlags desired_properties = choseProperties(flags);
		uint32_t available_type_bits = memory_requirement.memoryTypeBits;

		for (uint32_t i = 0; i < memory_types.size(); i++)
		{
			if ((memory_types[i].memory_properties_flags & desired_properties) == desired_properties &&
				(available_type_bits & memory_types[i].bit) != 0 &&
				!memory_heaps[memory_types[i].heap_index].full)
			{
				return i;
			}
		}

		Log::error("Failed to find suitable memory type");
		return 0;
	}

	std::string VK_Allocator::memoryTypeToString(const uint32_t mem_type_index)
	{
		VkMemoryPropertyFlags flags = device->getPhysicalDevice().getMemoryProperties().memoryTypes[mem_type_index].
			propertyFlags;
		std::string type;
		bool need_separator = false;
		std::string separator = " & ";
		if (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT & flags)
		{
			type += "\"Device local\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & flags)
		{
			if (need_separator)
				type += separator;
			type += "\"Host visible\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_CACHED_BIT & flags)
		{
			if (need_separator)
				type += separator;
			type += "\"Host chached\"";
			need_separator = true;
		}
		if (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & flags)
		{
			if (need_separator)
				type += separator;
			type += "\"Host coherent\"";
		}
		return type;
	}

	std::string VK_Allocator::allocToString(const Allocation& alloc)
	{
		return "Alloc#" + std::to_string(alloc.id) + " " + std::to_string(alloc.size) + " bytes of " +
			memoryTypeToString(alloc.block->memory_type_index) +
			" at position " + std::to_string(alloc.offset) + " in block " + std::to_string(alloc.block->index);
	}

	StagingAllocation VK_Allocator::createTempStagingBuffer(const void* data, size_t size)
	{
		VkBufferCreateInfo staging_buffer_info{};
		staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_info.size = size;
		staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkBuffer buffer;
		if (vkCreateBuffer(device->getHandle(), &staging_buffer_info, nullptr, &buffer) != VK_SUCCESS)
		{
			Log::error("failed to create buffer!");
		}

		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device->getHandle(), buffer, &requirements);

		Allocation staging_alloc = alloc(requirements, MEMORY_TYPE_FLAG_MAPPABLE);

		VkDeviceMemory& stagingBufferMemory = staging_alloc.block->memory;

		vkBindBufferMemory(device->getHandle(), buffer, staging_alloc.block->memory, staging_alloc.offset);
		StagingAllocation staging_buffer = StagingAllocation{staging_alloc, buffer};
		void* staging_buffer_data;
		vkMapMemory(device->getHandle(), staging_buffer.allocation.block->memory, staging_buffer.allocation.offset,
		            staging_buffer.allocation.size, 0, &staging_buffer_data);
		memcpy(staging_buffer_data, data, size);
		vkUnmapMemory(device->getHandle(), staging_buffer.allocation.block->memory);
		return staging_buffer;
	}

	void VK_Allocator::update(const VK_Buffer& buffer, const void* data, size_t size, size_t offset)
	{
		const Allocation& alloc = buffer.getAllocation();
		if (alloc.flags & MEMORY_TYPE_FLAG_MAPPABLE)
		{
			void* buffer_data;
			vkMapMemory(device->getHandle(), alloc.block->memory, alloc.offset + offset, alloc.size, 0, &buffer_data);
			size_t copy_size = (size_t)size;
			memcpy(buffer_data, data, copy_size);
			vkUnmapMemory(device->getHandle(), alloc.block->memory);
		}
		else
		{
			StagingAllocation staging_buffer = createTempStagingBuffer(data, size);
			staging_buffer.fence = &command_pool->getCurrentFence();
			copy(staging_buffer.vk_buffer, buffer.getHandle(), size, offset);
			delete_queue.emplace(staging_buffer);
		}
	}

	void VK_Allocator::updateRegions(VK_Image& image, const void* data, uint32_t data_texel_count,
	                                 ImageRegionUpdateInfo* update_infos, uint32_t update_count)
	{
		VkBufferImageCopy* regions_copy_infos = new VkBufferImageCopy[update_count];

		command_pool->begin();

		StagingAllocation staging_buffer = createTempStagingBuffer(data, data_texel_count * image.bytePerPixel());
		staging_buffer.fence = &command_pool->getCurrentFence();
		cmdBarrierTransitionImageLayout(command_pool, &image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		for (int i = 0; i < update_count; ++i)
		{
			HB_ASSERT(
				data_texel_count - update_infos->data_texel_offset >= (update_infos[i].size.x * update_infos[i].size.y *
					update_infos->size.z),
				"Data size must be greater than or equal to the size of the region to update.");

			regions_copy_infos[i] = {};
			regions_copy_infos[i].bufferOffset = update_infos[i].data_texel_offset * image.bytePerPixel();
			regions_copy_infos[i].bufferRowLength = 0;
			regions_copy_infos[i].bufferImageHeight = 0;

			regions_copy_infos[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			regions_copy_infos[i].imageSubresource.mipLevel = 0;
			regions_copy_infos[i].imageSubresource.baseArrayLayer = 0;
			regions_copy_infos[i].imageSubresource.layerCount = 1;
			regions_copy_infos[i].imageOffset = {
				update_infos[i].offset.x, update_infos[i].offset.y, update_infos[i].offset.z
			};
			regions_copy_infos[i].imageExtent = {
				update_infos[i].size.x, update_infos[i].size.y, update_infos[i].size.z
			};
		}
		vkCmdCopyBufferToImage(
			command_pool->getCurrentBuffer(),
			staging_buffer.vk_buffer,
			image.getHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			update_count,
			regions_copy_infos
		);
		delete_queue.emplace(staging_buffer);
		if (image.getMipLevelCount() > 1)
		{
			generateMipmaps(image);
		}
		else
		{
			cmdBarrierTransitionImageLayout(command_pool, &image, image.getDesiredLayout());
		}

		command_pool->end();
		command_pool->submit(QUEUE_FAMILY_GRAPHICS);


		delete regions_copy_infos;
	}

	void VK_Allocator::update(VK_Image& image, const void* data, uint32_t width, uint32_t depth, uint32_t height)
	{
		ImageRegionUpdateInfo info{};
		info.offset = vec3i(0, 0, 0);
		info.size = vec3u(width, height, depth);

		updateRegions(image, data, width * height * depth, &info, 1);
	}

	//https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
	Allocation VK_Allocator::alloc(VkMemoryRequirements mem_requirements, MEMORY_TYPE_FLAGS flags)
	{
		MemoryTypeInfo memory_type = memory_types[findMemoryTypeIndex(mem_requirements, flags)];

		for (Block* block : blocks[memory_type.heap_index])
		{
			if (block->memory_type_index != memory_type.index)
			{
				continue;
			}
			if (block->remaining < mem_requirements.size)
			{
				continue;
			}
			VkDeviceSize position = 0;

			for (const Allocation& current_alloc : block->allocations)
			{
				int32_t end_of_new_alloc = current_alloc.offset - (current_alloc.offset % mem_requirements.alignment);
				int32_t available = end_of_new_alloc - position;

				if (available >= static_cast<int32_t>(mem_requirements.size))
				{
					block->alloc_count++;
					block->remaining -= mem_requirements.size;
					const Allocation& new_alloc = *block->allocations.emplace(
						mem_requirements.size,
						position,
						block,
						flags,
						current_id++,
						memory_type.heap_index).first;
					//Log::debug("Allocated " + allocToString(new_alloc));
					return new_alloc;
				}
				else
				{
					position = current_alloc.offset + current_alloc.size;
					VkDeviceSize adjustement = position % mem_requirements.alignment;
					if (adjustement != 0)
					{
						position += mem_requirements.alignment - adjustement;
					}
				}
			}
			VkDeviceSize end_of_alloc = block->size - (block->size % mem_requirements.alignment);
			if (end_of_alloc - position >= mem_requirements.size)
			{
				block->alloc_count++;

				block->remaining -= mem_requirements.size;
				const Allocation& new_alloc = *block->allocations.emplace(
					mem_requirements.size,
					position,
					block,
					flags,
					current_id++,
					memory_type.heap_index).first;

				//Log::debug("Allocated " + allocToString(new_alloc));
				return new_alloc;
			}
		}
		//new block needed
		Block* block;
		auto block_it = block_cache.find(memory_type.index);
		if (block_it == block_cache.end())
		{
			uint32_t index = blocks[memory_type.heap_index].size();
			block = blocks[memory_type.heap_index].emplace_back(new Block{
				.size = mem_requirements.size > memory_heaps[memory_type.heap_index].block_size
					        ? mem_requirements.size
					        : memory_heaps[memory_type.heap_index].block_size,
				.memory = VK_NULL_HANDLE,
				.memory_type_index = memory_type.index,
				.index = index,
				.alloc_count = 0,
				.remaining = (mem_requirements.size > memory_heaps[memory_type.heap_index].block_size
					              ? mem_requirements.size
					              : memory_heaps[memory_type.heap_index].block_size) - mem_requirements.size
			});
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = block->size;
			allocInfo.memoryTypeIndex = memory_type.index;

			memory_heaps[memory_type.heap_index].used_size += block->size;
			float memory_used_ratio = static_cast<float>(memory_heaps[memory_type.heap_index].used_size) / static_cast<
				float>(memory_heaps[memory_type.heap_index].max_size);
			if (memory_used_ratio >= 0.9f)
			{
				memory_heaps[memory_type.heap_index].full = true;
			}
			Log::status(
				"Allocated " + std::to_string(memory_heaps[memory_type.heap_index].used_size / (1024 * 1024)) + "/" +
				std::to_string(memory_heaps[memory_type.heap_index].max_size / (1024 * 1024)) + "mb of heap#" +
				std::to_string(memory_type.heap_index) + ".\n Total used memory: " +
				std::to_string(
					(static_cast<float>(memory_heaps[memory_type.heap_index].used_size) / static_cast<float>(
						memory_heaps[memory_type.heap_index].max_size)) * 100) + "%");

			ApplicationInfo applicationInfo = Application::getInfo();
			VkMemoryAllocateFlagsInfo flagsInfo{};
			flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
			flagsInfo.flags = applicationInfo.required_extension_flags & VULKAN_REQUIRED_EXTENSION_RTX
				                  ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
				                  : 0;
			allocInfo.pNext = &flagsInfo;
			if (vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &block->memory) != VK_SUCCESS)
			{
				size_t total_memory = 0;
				for (auto& block : blocks)
				{
					for (auto& block : block.second)
					{
						total_memory += block->size;
					}
				}
				Log::message("Failed to allocate buffer memory! total memory used = " + total_memory);
				Log::error("failed to allocate buffer memory!");
			}
		}
		else
		{
			block = block_it->second;
			block->index = blocks[memory_type.heap_index].size();
			block_cache.erase(memory_type.index);
			blocks[memory_type.heap_index].emplace_back(block);
		}
		block->alloc_count++;
		const Allocation& new_alloc = *block->allocations.emplace(
			mem_requirements.size,
			VkDeviceSize(0),
			block,
			flags,
			current_id++,
			memory_type.heap_index).first;
		/*Log::debug(
				"Created block " + std::to_string(block->index) + " of type " + memoryTypeToString(block->memory_type) +
				" with " + std::to_string(block->size / 1048576.0f) + "mb");
		Log::debug("Allocated " + allocToString(new_alloc));*/
		return new_alloc;
	}

	void VK_Allocator::freeStagingBuffers(uint32_t frame)
	{
		while (!delete_queue.empty() &&
			(delete_queue.front().fence == VK_NULL_HANDLE || vkGetFenceStatus(device->getHandle(), delete_queue.front().fence->getHandle()) == VK_SUCCESS))
		{
			StagingAllocation staging_allocation = delete_queue.front();
			if (staging_allocation.vk_buffer != VK_NULL_HANDLE)
			{
				free(delete_queue.front().allocation);
				vkDestroyBuffer(device->getHandle(), delete_queue.front().vk_buffer, nullptr);
			}
			else if (staging_allocation.vk_image != VK_NULL_HANDLE)
			{
				vkDestroyImage(device->getHandle(), staging_allocation.vk_image, nullptr);
				free(delete_queue.front().allocation);
			}
			else if (staging_allocation.image != nullptr)
			{
				delete staging_allocation.image;
			}
			else if (staging_allocation.buffer != nullptr)
			{
				delete staging_allocation.buffer;
			}

			delete_queue.pop();
		}
	}

	void VK_Allocator::copy(VkBuffer src, VkBuffer dest, VkDeviceSize size, VkDeviceSize offset)
	{
		command_pool->begin();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = offset; // buffer offset not memory
		copyRegion.dstOffset = offset; // buffer offset not memory
		copyRegion.size = size;

		vkCmdCopyBuffer(command_pool->getCurrentBuffer(), src, dest, 1, &copyRegion);

		VkBufferMemoryBarrier dest_barrier = {};
		dest_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		dest_barrier.buffer = dest;
		dest_barrier.size = size;
		dest_barrier.offset = offset;
		dest_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		dest_barrier.dstAccessMask =
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
			VK_ACCESS_SHADER_READ_BIT |
			VK_ACCESS_TRANSFER_READ_BIT;
		dest_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		dest_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		dest_barrier.pNext = nullptr;
		int raytracingFlag = (device->getPhysicalDevice().getEnabledExtensionFlags() &
			                     EXTENSION_FLAG_RAY_TRACING_PIPELINE != 0)
			                     ? VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
			                     VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR
			                     : 0;
		vkCmdPipelineBarrier(command_pool->getCurrentBuffer(),
		                     VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     VK_PIPELINE_STAGE_TRANSFER_BIT |
		                     VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
		                     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
		                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
		                     raytracingFlag,
		                     0,
		                     0,
		                     nullptr,
		                     1,
		                     &dest_barrier,
		                     0,
		                     nullptr);


		command_pool->end();

		//todo use transfer queue
		command_pool->submit(QUEUE_FAMILY_GRAPHICS);
	}

	void VK_Allocator::cmdBarrierTransitionImageLayout(VK_CommandPool* command_pool, VK_Image* image,
	                                                   VkImageLayout new_layout)
	{
		VkImageLayout old_layout = image->getImageLayout();

		//block dstStage until srcStage is finished
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image->getHandle();
		barrier.subresourceRange.aspectMask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
			                                      new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
			                                      ? VK_IMAGE_ASPECT_DEPTH_BIT
			                                      : VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = image->getMipLevelCount();
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			VkFormatProperties format_properties;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (image->getVkFormat() == VK_FORMAT_D32_SFLOAT_S8_UINT ||
				image->getVkFormat() == VK_FORMAT_D24_UNORM_S8_UINT)
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkPipelineStageFlags sourceStage; //producer stage
		VkPipelineStageFlags destinationStage; // consumer stage

		//    TOP_OF_PIPE_BIT start of pipeline
		//    DRAW_INDIRECT_BIT
		//    VERTEX_INPUT_BIT
		//    VERTEX_SHADER_BIT
		//    TESSELLATION_CONTROL_SHADER_BIT
		//    TESSELLATION_EVALUATION_SHADER_BIT
		//    GEOMETRY_SHADER_BIT
		//    FRAGMENT_SHADER_BIT
		//    EARLY_FRAGMENT_TESTS_BIT
		//    LATE_FRAGMENT_TESTS_BIT
		//    COLOR_ATTACHMENT_OUTPUT_BIT
		//    TRANSFER_BIT
		//    COMPUTE_SHADER_BIT
		//    BOTTOM_OF_PIPE_BIT end of pipeline

		//refer to https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPipelineStageFlagBits.html for stages meanings
		switch (old_layout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			//Don't wait for anything because image is not in use because it has layout undefined so start operation now
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_GENERAL:
			{
				uint32_t extra_source_stage = (device->getPhysicalDevice().getEnabledExtensionFlags() & EXTENSION_FLAG_RAY_TRACING_PIPELINE) != 0
					                              ? VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR
					                              : 0;
				barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
				sourceStage = extra_source_stage | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
			}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		default:
			Log::error("Unsupported layout transition! (unsuported)" + VK_Utils::layoutToString(old_layout) + " -> " + VK_Utils::layoutToString(new_layout));
			break;
		}
		switch (new_layout)
		{
		case VK_IMAGE_LAYOUT_GENERAL:
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		default:
			Log::error("Unsupported layout transition! " + VK_Utils::layoutToString(old_layout) + " -> (unsuported)" + VK_Utils::layoutToString(new_layout));
			return;
		}

		vkCmdPipelineBarrier(
			command_pool->getCurrentBuffer(),
			sourceStage,
			destinationStage,
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

	VK_Fence* VK_Allocator::blitImage(VK_Image& src, VK_Image& dest)
	{
		VK_Fence* fence = &command_pool->getCurrentFence();;
		VkFormatProperties src_format_properties;
		VkFormatProperties dest_format_properties;
		vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice().getHandle(), src.getVkFormat(),
		                                    &src_format_properties);
		vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice().getHandle(), dest.getVkFormat(),
		                                    &dest_format_properties);

		if (src_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT &&
			dest_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)
		{
			VkImageBlit region{};
			region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.srcSubresource.layerCount = 1;
			region.srcOffsets[1] = {0, 0, 1};
			region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.dstSubresource.layerCount = 1;
			region.dstOffsets[1] = {0, 0, 1};

			command_pool->begin();
			cmdBarrierTransitionImageLayout(command_pool, &src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			cmdBarrierTransitionImageLayout(command_pool, &dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkCmdBlitImage(
				command_pool->getCurrentBuffer(),
				src.getHandle(), src.getImageLayout(),
				dest.getHandle(), dest.getImageLayout(),
				1,
				&region,
				VK_FILTER_NEAREST
			);

			cmdBarrierTransitionImageLayout(command_pool, &src, src.getDesiredLayout());
			cmdBarrierTransitionImageLayout(command_pool, &dest, dest.getDesiredLayout());

			command_pool->end();
			command_pool->submit(QUEUE_FAMILY_GRAPHICS);
		}
		else
		{
			Log::error("format is not suported for blitting");
		}
		return fence;
	}

	void VK_Allocator::destroyStagingAllocation(const StagingAllocation& allocation)
	{
		delete_queue.emplace(allocation);
	}

	void VK_Allocator::generateMipmaps(VK_Image& image)
	{
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice().getHandle(), image.getVkFormat(),
		                                    &format_properties);

		if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)
		{
			VkFilter filter = VK_FILTER_NEAREST;
			if (format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
			{
				filter = VK_FILTER_LINEAR;
			}
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = image.getHandle();
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			int32_t mipWidth = image.getWidth();
			int32_t mipHeight = image.getHeight();
			int32_t mipDepth = image.getDepth();

			for (uint32_t i = 1; i < image.getMipLevelCount(); i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(command_pool->getCurrentBuffer(),
				                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				                     0, nullptr,
				                     0, nullptr,
				                     1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0] = {0, 0, 0};
				blit.srcOffsets[1] = {mipWidth, mipHeight, mipDepth};
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = {0, 0, 0};
				blit.dstOffsets[1] = {
					mipWidth > 1 ? mipWidth / 2 : 1,
					mipHeight > 1 ? mipHeight / 2 : 1,
					mipDepth > 1 ? mipDepth / 2 : 1
				};
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;


				vkCmdBlitImage(command_pool->getCurrentBuffer(),
				               image.getHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				               image.getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				               1, &blit,
				               filter);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = image.getDesiredLayout();
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(command_pool->getCurrentBuffer(),
				                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				                     0, nullptr,
				                     0, nullptr,
				                     1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
				if (mipDepth > 1) mipDepth /= 2;
			}

			barrier.subresourceRange.baseMipLevel = image.getMipLevelCount() - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = image.getDesiredLayout();
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_pool->getCurrentBuffer(),
			                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			                     0, nullptr,
			                     0, nullptr,
			                     1, &barrier);


			image.setImageLayout(image.getDesiredLayout());
		}
		//todo compute shader
		else
		{
			Log::error("format is not suported for blitting and mipmapping");
		}
	}

	void VK_Allocator::copy(VkBuffer src, VK_Image* dest, VkImageLayout dst_end_layout, VkBufferImageCopy region)
	{
		command_pool->begin();
		cmdBarrierTransitionImageLayout(command_pool, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkCmdCopyBufferToImage(
			command_pool->getCurrentBuffer(),
			src,
			dest->getHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		if (dest->getMipLevelCount() > 1)
		{
			generateMipmaps(*dest);
		}
		else
		{
			cmdBarrierTransitionImageLayout(command_pool, dest, dst_end_layout);
		}


		command_pool->end();
		command_pool->submit(QUEUE_FAMILY_GRAPHICS);
	}

	void VK_Allocator::copy(VK_Image* src, VkImageLayout src_end_layout, VK_Image* dest, VkImageLayout dst_end_layout)
	{
		VkImageAspectFlagBits src_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
		//todo change for depth if image is depth or stencil
		VkImageAspectFlagBits dst_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
		command_pool->begin();
		cmdBarrierTransitionImageLayout(command_pool, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		cmdBarrierTransitionImageLayout(command_pool, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
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
		cmdBarrierTransitionImageLayout(command_pool, dest, dst_end_layout);
		cmdBarrierTransitionImageLayout(command_pool, src, src_end_layout);
		command_pool->end();
		command_pool->submit(QUEUE_FAMILY_GRAPHICS);
	}

	void VK_Allocator::free(const Allocation& allocation)
	{
		//Log::debug("Freeing Alloc#" + std::to_string(allocation.id));
		Block* block = allocation.block;

		uint32_t heap_index = allocation.heap_index;
		//Log::debug("Freed " + allocToString(allocation));
		if (allocation.block->memory_type_index)
			if (allocation.block->alloc_count == 1)
			{
				auto it = blocks[heap_index].erase(blocks[heap_index].begin() + allocation.block->index);
				auto block_pool_it = block_cache.find(allocation.block->memory_type_index);
				if (block_pool_it == block_cache.end())
				{
					block->remaining = block->size;
					block->alloc_count = 0;
					block->allocations.clear();
					block_cache.emplace(block->memory_type_index, block);
				}
				else
				{
					vkFreeMemory(device->getHandle(), allocation.block->memory, nullptr);
					delete block;
				}
				for (; it != blocks[heap_index].end(); ++it)
				{
					(*it)->index--;
				}
			}
			else
			{
				block->alloc_count--;
				allocation.block->allocations.erase(allocation);
			}
	}


	void VK_Allocator::setImageLayout(VK_Image* image, VkImageLayout newLayout)
	{
		command_pool->begin();
		VK_Allocator::cmdBarrierTransitionImageLayout(command_pool, image, newLayout);
		command_pool->end();
		command_pool->submit(QUEUE_FAMILY_GRAPHICS);
	}
}
