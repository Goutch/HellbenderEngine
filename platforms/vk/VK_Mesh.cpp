#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"

#include "VK_Buffer.h"
#include "VK_Allocator.h"
#include "VK_Renderer.h"
#include "VK_StorageBuffer.h"

namespace HBE
{
	VK_Mesh::VK_Mesh(VK_Renderer* renderer, VK_CommandPool* command_pool, const MeshInfo& info)
	{
		this->info = info;
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->command_pool = command_pool;
		if ((info.flags & MESH_FLAG_USED_IN_RAYTRACING) != 0)
		{
			this->extra_usages |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			this->extra_usages |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		}
		if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
		{
			this->extra_usages |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		for (size_t i = 0; i < info.attribute_info_count; ++i)
		{
			size_t number_of_buffers = (info.attribute_infos[i].flags & VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS) != 0 ? MAX_FRAMES_IN_FLIGHT : 1;
			buffers.emplace(info.attribute_infos[i].location, std::vector<VK_Buffer*>(number_of_buffers, nullptr));
			attributes_locations.emplace(info.attribute_infos[i].location, info.attribute_infos[i]);
		}
	}

	void VK_Mesh::setBuffer(uint32_t location, const void* vertices, size_t count)
	{
		this->vertex_count = count;
		if (count == 0) return;
		auto it = buffers.find(location);
		if (it == buffers.end())
		{
			Log::warning("Trying to set a mesh buffer at location#" + std::to_string(location) + " but no such location exists");
			return;
		}
		if (count == 0)
		{
			Log::warning("Trying to set a mesh buffer with vertex count = 0");
			return;
		}
		VertexAttributeInfo& attribute_info = attributes_locations[location];
		VkDeviceSize buffer_size = attribute_info.size * count;

		if ((attribute_info.flags & VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS) == VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS)
		{
			if (buffers[location][renderer->getCurrentFrame()])
			{
				StagingAllocation staging_allocation{};
				staging_allocation.buffer = buffers[location][renderer->getCurrentFrame()];
				staging_allocation.fence = &command_pool->getCurrentFence();
				device->getAllocator()->destroyStagingAllocation(staging_allocation);
			}
			buffers[location][renderer->getCurrentFrame()] = new VK_Buffer(device,
			                                                               vertices,
			                                                               buffer_size,
			                                                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
			                                                               (attribute_info.preferred_memory_type_flags));
			if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
			{
				auto it = storage_buffers.find(location);
				if (it != storage_buffers.end())
				{
					delete it->second;
					it->second = new VK_StorageBuffer(device, buffers[location][renderer->getCurrentFrame()], attribute_info.size, count, false);
				}
				else
				{
					storage_buffers[location] = new VK_StorageBuffer(device, buffers[location][renderer->getCurrentFrame()], attribute_info.size, count, false);
				}
			}
		}
		else
		{
			//todo: add to delete queue instead of waiting and deleting immediately
			device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
			if (buffers[location][0])
			{
				delete buffers[location][0];
			}
			buffers[location][0] = new VK_Buffer(device,
			                                     vertices,
			                                     buffer_size,
			                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
			                                     attribute_info.preferred_memory_type_flags);

			if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
			{
				auto it = storage_buffers.find(location);
				if (it != storage_buffers.end())
				{
					delete it->second;
					it->second = new VK_StorageBuffer(device, buffers[location][0], attribute_info.size, count, false);
				}
				else
				{
					storage_buffers[location] = new VK_StorageBuffer(device, buffers[location][0], attribute_info.size, count, false);
				}
			}
		}
	}



	void VK_Mesh::setInstanceBuffer(uint32_t location, const void* data, size_t count)
	{
		this->instance_count = count;
		if (count == 0) return;
		HB_ASSERT(buffers.find(location) != buffers.end(), "Trying to set an instance buffer at a location that were not included in the attribute infos");
		VertexAttributeInfo& binding_info = attributes_locations[location];
		VkDeviceSize buffer_size = binding_info.size * count;

		if ((binding_info.flags & VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS) == VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS)
		{
			renderer->waitCurrentFrame();
			if (buffers[location][renderer->getCurrentFrame()])
			{
				delete buffers[location][renderer->getCurrentFrame()];
			}
			buffers[location][renderer->getCurrentFrame()] = new VK_Buffer(device,
			                                                               data,
			                                                               buffer_size,
			                                                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
			                                                               binding_info.preferred_memory_type_flags);
		}
		else
		{
			device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
			if (buffers[location][0])
			{
				delete buffers[location][0];
			}

			buffers[location][0] = new VK_Buffer(device,
			                                     data,
			                                     buffer_size,
			                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
			                                     binding_info.preferred_memory_type_flags);
		}
	}

	VK_Mesh::~VK_Mesh()
	{
		//todo: use allocator delete queue9
		VK_Allocator* allocator = device->getAllocator();
		for (size_t i = 0; i < attributes_locations.size(); ++i)
		{
			for (size_t j = 0; j < buffers[i].size(); ++j)
			{

				if (buffers[i][j])
					delete buffers[i][j];
			}
		}
		buffers.clear();
		if (indices_buffer)
			delete indices_buffer;

		if (indices_storage_buffer)
			delete indices_storage_buffer;
		for (auto& it : storage_buffers)
		{
			delete it.second;
		}
	}

	void VK_Mesh::setVertexIndices(const void* data, size_t count, size_t element_size)
	{
		//todo: remove wait here and delete in allocator
		device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
		if (indices_buffer)
		{
			delete indices_buffer;
		}
		VkDeviceSize buffer_size = element_size * count;


		indices_buffer = new VK_Buffer(device,
		                               data,
		                               buffer_size,
		                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT | extra_usages,
		                               MEMORY_TYPE_FLAG_NONE);

		if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
		{
			if (indices_storage_buffer)
			{
				delete indices_storage_buffer;
			}
			indices_storage_buffer = new VK_StorageBuffer(device, indices_buffer, element_size, count, false);
		}
		index_count = count;
	}

	void VK_Mesh::setVertexIndices(const uint16_t* data, size_t count)
	{
		indices_type = INDICES_TYPE_UINT16;
		setVertexIndices(data, count, sizeof(data[0]));
	}

	void VK_Mesh::setVertexIndices(const uint32_t* data, size_t count)
	{
		indices_type = INDICES_TYPE_UINT32;
		setVertexIndices(data, count, sizeof(data[0]));
	}

	void VK_Mesh::bind() const
	{
		if (bound)
			return;
		bound = true;
		//todo: cleanup this
		VkBuffer* flat_buffers = new VkBuffer[buffers.size()];
		VkDeviceSize* offsets = new VkDeviceSize[buffers.size()];
		int i = 0;
		for (auto buffer : buffers)
		{
			if ((attributes_locations.at(buffer.first).flags & VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS) == VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS)
			{
				flat_buffers[i] = buffer.second[renderer->getCurrentFrame()]->getHandle();
			}
			else
			{
				flat_buffers[i] = buffer.second[0]->getHandle();
			}
			offsets[i] = 0;
			i++;
		}

		vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(),
		                       0,
		                       buffers.size(),
		                       flat_buffers,
		                       offsets);
		if (indices_type != INDICES_TYPE_NONE)
		{
			vkCmdBindIndexBuffer(command_pool->getCurrentBuffer(),
			                     indices_buffer->getHandle(),
			                     0,
			                     indices_type == INDICES_TYPE_UINT32 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
		}
		delete[] offsets;
		delete[] flat_buffers;
	}

	void VK_Mesh::unbind() const
	{
		bound = false;
	}

	const VK_Buffer* VK_Mesh::getBuffer(uint32_t binding, uint32_t frame) const
	{
		return buffers.at(binding).at(frame);
	}

	const VK_Buffer* VK_Mesh::getIndicesBuffer() const
	{
		return indices_buffer;
	}

	StorageBuffer* VK_Mesh::getAttributeStorageBuffer(uint32_t location) const
	{
		HB_ASSERT(info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER,
		          "Trying to get a storage buffer from a mesh that does not have the flag MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER");

		auto it = storage_buffers.find(location);
		HB_ASSERT(it != storage_buffers.end(), "Trying to get a storage buffer from a mesh that does not have an attribute at location:" + std::to_string(location));

		return it->second;
	}

	StorageBuffer* VK_Mesh::getIndicesStorageBuffer() const
	{
		HB_ASSERT(info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER,
		          "Trying to get a storage buffer from a mesh that does not have the flag MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER");
		HB_ASSERT(hasIndexBuffer(), "Trying to get a storage buffer from a mesh that does not have an index buffer");
		return indices_storage_buffer;
	}
}
