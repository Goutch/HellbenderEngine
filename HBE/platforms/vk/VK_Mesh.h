#pragma once

#include "core/resource/Mesh.h"
#include "vector"
#include "vulkan/vulkan.h"
#include "set"
#include "VK_CommandPool.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_Renderer;

	class VK_Mesh : public Mesh {
		const VK_CommandPool *command_pool;
		VK_Device *device = nullptr;
		VK_Renderer *renderer = nullptr;
		VK_Buffer *indices_buffer = nullptr;
		mutable std::unordered_map<uint32_t, std::vector<VK_Buffer *>> buffers;
	public:
		VK_Mesh(VK_Renderer *renderer, const VK_CommandPool *command_pool, const MeshInfo &info);
		~VK_Mesh() override;
		void setVertexIndices(const uint32_t *data, size_t count) override;
		void setVertexIndices(const uint16_t *data, size_t count) override;
		void setVertexIndices(const void *data, size_t count, size_t element_size);
		void setBuffer(uint32_t binding, const void *vertices, size_t count) override;
		void setInstanceBuffer(uint32_t binding, const void *data, size_t count) override;
		void bind() const override;
		void unbind() const override;
	};

}
