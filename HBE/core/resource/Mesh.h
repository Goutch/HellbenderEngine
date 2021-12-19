#pragma once

#include "Core.h"
#include <vector>
#include "glm/glm.hpp"
#include "Resource.h"
#include "GraphicPipeline.h"

namespace HBE {
	class MeshLayout;

	typedef uint32_t MESH_FLAGS;
	enum MESH_FLAG {
		MESH_FLAG_NONE = 0,
	};

	struct MeshInfo {
		const VertexBindingInfo *binding_infos = nullptr;
		size_t binding_info_count = 0;
		MESH_FLAGS flags = MESH_FLAG_NONE;
	};

	class HB_API Mesh : public Resource {
	protected:
		uint32_t vertex_count = 0;
		uint32_t index_count = 0;
		bool has_index_buffer = false;
		uint32_t instance_count = 1;
		std::unordered_map<uint32_t, VertexBindingInfo> bindings;
	public:
		virtual ~Mesh() = default;
		uint32_t getVertexCount() const;
		uint32_t getIndexCount() const;
		uint32_t getInstanceCount() const;
		bool hasIndexBuffer() const;
		virtual void setVertexIndices(const std::vector<uint32_t> &data) = 0;
		virtual void setBuffer(uint32_t binding, const void *vertices, size_t count) = 0;
		virtual void setInstanceBuffer(uint32_t binding, const void *data, size_t count) = 0;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

	};
}

