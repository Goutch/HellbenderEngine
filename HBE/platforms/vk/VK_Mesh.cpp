
#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"

#include "VK_Buffer.h"
#include "VK_Allocator.h"
#include "VK_CONSTANTS.h"
#include "VK_Renderer.h"

namespace HBE {
	VK_Mesh::VK_Mesh(VK_Renderer *renderer, const VK_CommandPool *command_pool, const MeshInfo &info) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->command_pool = command_pool;

		for (size_t i = 0; i < info.binding_info_count; ++i) {
			buffers.emplace(info.binding_infos[i].binding, std::vector<VK_Buffer *>(
					(info.binding_infos[i].flags & VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) == VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS ?
					MAX_FRAMES_IN_FLIGHT : 1, nullptr));
			bindings.emplace(info.binding_infos[i].binding, info.binding_infos[i]);
		}
	}

	void VK_Mesh::setBuffer(uint32_t binding, const void *vertices, size_t count) {
		this->vertex_count = count;
		if (count == 0) {
			Log::warning("Trying to set a mesh buffer with vertex count = 0");
			return;
		}
		VertexBindingInfo &binding_info = bindings[binding];
		VkDeviceSize buffer_size = binding_info.size * count;

		if ((binding_info.flags & VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) == VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) {
			renderer->waitCurrentFrame();
			if (buffers[binding][renderer->getCurrentFrame()]) {
				delete buffers[binding][renderer->getCurrentFrame()];
			}
			buffers[binding][renderer->getCurrentFrame()] = new VK_Buffer(device,
																		  vertices,
																		  buffer_size,
																		  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
																		  ((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
																		  ALLOC_FLAG_MAPPABLE :
																		  ALLOC_FLAG_NONE);

		} else {
			device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
			if (buffers[binding][0]) {
				delete buffers[binding][0];
			}
			buffers[binding][0] = new VK_Buffer(device,
												vertices,
												buffer_size,
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
												((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
												ALLOC_FLAG_MAPPABLE :
												ALLOC_FLAG_NONE);
		}
	}

	void VK_Mesh::setInstanceBuffer(uint32_t binding, const void *data, size_t count) {
		this->instance_count = count;
		VertexBindingInfo &binding_info = bindings[binding];
		VkDeviceSize buffer_size = binding_info.size * count;

		if ((binding_info.flags & VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) == VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) {
			renderer->waitCurrentFrame();
			if (buffers[binding][renderer->getCurrentFrame()]) {
				delete buffers[binding][renderer->getCurrentFrame()];
			}
			buffers[binding][renderer->getCurrentFrame()] = new VK_Buffer(device,
																		  data,
																		  buffer_size,
																		  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
																		  ((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
																		  ALLOC_FLAG_MAPPABLE :
																		  ALLOC_FLAG_NONE);

		} else {
			device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
			if (buffers[binding][0]) {
				delete buffers[binding][0];
			}

			buffers[binding][0] = new VK_Buffer(device,
												data,
												buffer_size,
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
												((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
												ALLOC_FLAG_MAPPABLE :
												ALLOC_FLAG_NONE);
		}

	}

	VK_Mesh::~VK_Mesh() {
		;
		for (size_t i = 0; i < bindings.size(); ++i) {
			for (size_t j = 0; j < buffers[i].size(); ++j) {
				if (buffers[i][j])
					delete buffers[i][j];
			}
		}
		buffers.clear();
		if (has_index_buffer)
			delete indices_buffer;
	}

	void VK_Mesh::setVertexIndices(const std::vector<uint32_t> &data) {
		device->getQueue(QUEUE_FAMILY_GRAPHICS)->wait();
		if (has_index_buffer) {
			delete indices_buffer;
		}
		this->has_index_buffer = true;
		this->index_count = data.size();
		VkDeviceSize buffer_size = sizeof(data[0]) * data.size();


		indices_buffer = new VK_Buffer(device,
									   reinterpret_cast<const void *>(data.data()),
									   buffer_size,
									   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
									   ALLOC_FLAGS::ALLOC_FLAG_NONE);
	}

	void VK_Mesh::bind() const {

		VkBuffer *flat_buffers = new VkBuffer[buffers.size()];
		VkDeviceSize *offsets = new VkDeviceSize[buffers.size()];
		int i = 0;
		for (auto buffer: buffers) {
			if ((bindings.at(buffer.first).flags & VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) == VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) {
				flat_buffers[i] = buffer.second[renderer->getCurrentFrame()]->getHandle();
			} else {
				flat_buffers[i] = buffer.second[0]->getHandle();
			}


			offsets[i] = 0;
			i++;
		}

		vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(), 0, buffers.size(), flat_buffers, offsets);
		if (has_index_buffer)
			vkCmdBindIndexBuffer(command_pool->getCurrentBuffer(), indices_buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		delete offsets;
		delete flat_buffers;
	}

	void VK_Mesh::unbind() const {

	}


}



