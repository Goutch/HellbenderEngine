#pragma once

#include "Core.h"
#include <vector>
#include "glm/glm.hpp"
#include "Resource.h"

namespace HBE {
	class VertexLayout;

	enum MESH_FLAGS {
		MESH_FLAG_NONE = 0,
	};
	struct MeshInfo {
		VertexLayout *layout = nullptr;
		MESH_FLAGS flags = MESH_FLAG_NONE;
	};

	class HB_API Mesh : public Resource {
	protected:
		uint32_t vertex_count = 0;
		uint32_t index_count = 0;
		bool has_index_buffer = false;
		uint32_t instance_count = 1;
		VertexLayout *layout;
	public:
		virtual ~Mesh() = default;
		uint32_t getVertexCount() const;
		uint32_t getIndexCount() const;
		uint32_t getInstanceCount() const;
		bool hasIndexBuffer() const;
		virtual void setVertices(uint32_t position, const void *vertices, size_t count) = 0;
		virtual void setIndices(const std::vector<uint32_t> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<int> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<float> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<vec2> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<vec3> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<vec4> &data) = 0;
		virtual void setBuffer(uint32_t position, const std::vector<uint32_t> &data) = 0;
		virtual void setInstancedBuffer(uint32_t position, const std::vector<mat4> &data) = 0;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

	};
}

