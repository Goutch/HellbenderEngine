
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
		if ((info.flags & MESH_FLAG_USED_IN_RAYTRACING) != 0) {

			this->extra_usages |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			this->extra_usages |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		}

		for (size_t i = 0; i < info.binding_info_count; ++i) {
			size_t number_of_buffers = (info.binding_infos[i].flags & VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS) != 0 ?
									   MAX_FRAMES_IN_FLIGHT : 1;
			buffers.emplace(info.binding_infos[i].binding, std::vector<VK_Buffer *>(number_of_buffers, nullptr));
			bindings.emplace(info.binding_infos[i].binding, info.binding_infos[i]);
		}
	}

	void VK_Mesh::setBuffer(uint32_t binding, const void *vertices, size_t count) {
		this->vertex_count = count;
		auto it = buffers.find(binding);
		if (it == buffers.end()) {
			Log::warning("Trying to set a mesh buffer at binding#" + std::to_string(binding) + " but no such binding exists");
			return;
		}
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
																		  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
																		  ((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
																		  ALLOC_FLAG_MAPPABLE :
																		  ALLOC_FLAG_NONE);

		} else {
			device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
			if (buffers[binding][0]) {
				delete buffers[binding][0];
			}
			buffers[binding][0] = new VK_Buffer(device,
												vertices,
												buffer_size,
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
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
																		  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
																		  ((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
																		  ALLOC_FLAG_MAPPABLE :
																		  ALLOC_FLAG_NONE);

		} else {
			device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
			if (buffers[binding][0]) {
				delete buffers[binding][0];
			}

			buffers[binding][0] = new VK_Buffer(device,
												data,
												buffer_size,
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
												((binding_info.flags & VERTEX_BINDING_FLAG_FAST_WRITE) == VERTEX_BINDING_FLAG_FAST_WRITE) ?
												ALLOC_FLAG_MAPPABLE :
												ALLOC_FLAG_NONE);
		}

	}

	VK_Mesh::~VK_Mesh() {
		for (size_t i = 0; i < bindings.size(); ++i) {
			for (size_t j = 0; j < buffers[i].size(); ++j) {
				if (buffers[i][j])
					delete buffers[i][j];
			}
		}
		buffers.clear();
		if (indices_buffer)
			delete indices_buffer;
	}

	void VK_Mesh::setVertexIndices(const void *data, size_t count, size_t element_size) {
		device->getQueue(QUEUE_FAMILY_GRAPHICS).wait();
		if (indices_buffer) {
			delete indices_buffer;
		}
		VkDeviceSize buffer_size = element_size * count;


		indices_buffer = new VK_Buffer(device,
									   reinterpret_cast<const void *>(data),
									   buffer_size,
									   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | extra_usages,
									   ALLOC_FLAG_NONE);
		index_count = count;
	}

	void VK_Mesh::setVertexIndices(const uint16_t *data, size_t count) {
		indices_type = INDICES_TYPE_UINT16;
		setVertexIndices(data, count, sizeof(data[0]));
	}

	void VK_Mesh::setVertexIndices(const uint32_t *data, size_t count) {
		indices_type = INDICES_TYPE_UINT32;
		setVertexIndices(data, count, sizeof(data[0]));
	}

	void VK_Mesh::bind() const {
		//todo: cleanup this
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

		vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(),
							   0,
							   buffers.size(),
							   flat_buffers,
							   offsets);
		if (indices_type != INDICES_TYPE_NONE) {
			vkCmdBindIndexBuffer(command_pool->getCurrentBuffer(),
								 indices_buffer->getHandle(),
								 0,
								 indices_type == INDICES_TYPE_UINT32 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
		}
		delete[] offsets;
		delete[] flat_buffers;
	}

	void VK_Mesh::unbind() const {

	}

	const VK_Buffer *VK_Mesh::getBuffer(uint32_t binding, uint32_t frame) const {
		return buffers.at(binding).at(frame);
	}

	const VK_Buffer *VK_Mesh::getIndicesBuffer() const {
		return indices_buffer;
	}


}



