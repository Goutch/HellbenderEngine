
#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"
#include "VK_VertexLayout.h"
#include "VK_Buffer.h"
#include "VK_Allocator.h"

namespace HBE {
	VK_Mesh::VK_Mesh(VK_Device *device, const VK_CommandPool *command_pool) {
		this->device = device;
		this->command_pool = command_pool;
	}

	void VK_Mesh::setVertices(uint32_t position, void *vertices, size_t count, const VertexLayout *layout) {

		this->vertex_count = count;
		VkDeviceSize buffer_size = layout->getBytesPerVertex() * count;
		if (buffers.find(position) != buffers.end()) {
			delete buffers[position];
			buffers.erase(position);
		}

		buffers[position] = new VK_Buffer(device,
										  vertices,
										  buffer_size,
										  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}

	VK_Mesh::~VK_Mesh() {
		for (auto buffer: buffers) {
			delete buffer.second;
		}
		buffers.clear();
		delete indices_buffer;
	}

	void VK_Mesh::setIndices(const std::vector<unsigned int> &indices) {
		this->has_index_buffer = true;
		this->index_count = indices.size();
		VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
		indices_buffer = new VK_Buffer(device,
									   reinterpret_cast<const void *>(indices.data()),
									   buffer_size,
									   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
									   VK_Buffer::NONE);
	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<int> &data) {

	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<float> &data) {

	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<vec2> &data) {

	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<vec3> &data) {

	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<vec4> &data) {

	}

	void VK_Mesh::setBuffer(uint32_t position, const std::vector<uint32_t> &data) {

	}

	void VK_Mesh::setInstancedBuffer(uint32_t position, const std::vector<mat4> &data) {

	}

	void VK_Mesh::bind() const {

		VkBuffer *flat_buffers = new VkBuffer[buffers.size()];
		VkDeviceSize *offsets = new VkDeviceSize[buffers.size()];
		int i = 0;
		for (auto buffer: buffers) {
			flat_buffers[i] = buffer.second->getHandle();
			offsets[i] = 0;
			i++;
		}

		vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(), 0, buffers.size(), flat_buffers, offsets);
		vkCmdBindIndexBuffer(command_pool->getCurrentBuffer(), indices_buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		delete offsets;
		delete flat_buffers;
	}

	void VK_Mesh::unbind() const {

	}


}



