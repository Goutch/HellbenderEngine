#pragma once

#include "core/resource/Mesh.h"
#include "vector"
#include "vulkan/vulkan.h"
#include "set"
#include "VK_CommandPool.h"
#include "core/resource/StorageBuffer.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_Renderer;

	class VK_Mesh : public Mesh {
		VK_CommandPool *command_pool;
		VK_Device *device = nullptr;
		VK_Renderer *renderer = nullptr;
		VK_Buffer *indices_buffer = nullptr;
		mutable std::unordered_map<uint32_t, std::vector<VK_Buffer *>> buffers;
		VkBufferUsageFlags extra_usages = 0;
		mutable bool bound = false;
		MeshInfo info;
		std::unordered_map<uint32_t, StorageBuffer *> storage_buffers;
		StorageBuffer *indices_storage_buffer = nullptr;
	public:
		VK_Mesh(VK_Renderer *renderer, VK_CommandPool *command_pool, const MeshInfo &info);

		~VK_Mesh() override;

		void setVertexIndices(const uint32_t *data, size_t count) override;

		void setVertexIndices(const uint16_t *data, size_t count) override;

		void setVertexIndices(const void *data, size_t count, size_t element_size);

		void setBuffer(uint32_t location, const void *vertices, size_t count) override;

		void setInstanceBuffer(uint32_t location, const void *data, size_t count) override;

		const VK_Buffer *getBuffer(uint32_t binding, uint32_t frame = 0) const;

		const VK_Buffer *getIndicesBuffer() const;

		StorageBuffer *getIndicesStorageBuffer() const override;

		StorageBuffer *getAttributeStorageBuffer(uint32_t location) const override;

		void bind() const override;

		void unbind() const override;
	};

}
