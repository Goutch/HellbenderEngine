
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

    void VK_Mesh::setVertices(int position, void *vertices, size_t count, const VertexLayout *layout) {
        this->vertex_count = count;
        VkDeviceSize buffer_size = layout->getBytesPerVertex() * count;
        if (buffers.find(position) != buffers.end()) {
            delete buffers[position];
            buffers.erase(position);
        }




        VK_Buffer staging_buffer = VK_Buffer(device,
                                       buffer_size,
                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        const Allocation &allocation = staging_buffer.getAllocation();
        VkDeviceMemory& stagingBufferMemory = allocation.block.memory;
        void *data;
        vkMapMemory(device->getHandle(), stagingBufferMemory, allocation.offset, buffer_size, 0, &data);
        memcpy(data, vertices, (size_t) allocation.size);
        vkUnmapMemory(device->getHandle(), stagingBufferMemory);

        buffers[position] = new VK_Buffer(device,
                                          buffer_size,
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        buffers[position]->copy(&staging_buffer);


    }

    VK_Mesh::~VK_Mesh() {
        for (auto buffer: buffers) {
            delete buffer.second;
        }
        buffers.clear();
    }

    void VK_Mesh::setIndices(const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) {

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

        delete offsets;
        delete flat_buffers;
    }

    void VK_Mesh::unbind() const {

    }


}



