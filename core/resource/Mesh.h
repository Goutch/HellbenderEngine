#pragma once

#include "Core.h"
#include <vector>
#include "glm/glm.hpp"
#include "Resource.h"
#include "unordered_map"
#include "string"
namespace HBE {

	typedef uint32_t MESH_FLAGS;
	enum MESH_FLAG {
		MESH_FLAG_NONE = 0,
		MESH_FLAG_USED_IN_RAYTRACING = 1 << 0,
	};

	struct MeshInfo {
		const VertexAttributeInfo *attribute_infos = nullptr;
		size_t attribute_info_count = 0;
		MESH_FLAGS flags = MESH_FLAG_NONE;
	};

	class HB_API Mesh : public Resource {
	protected:
		enum INDICES_TYPE {
			INDICES_TYPE_NONE,
			INDICES_TYPE_UINT16,
			INDICES_TYPE_UINT32,
		};
		uint32_t vertex_count = 0;
		uint32_t index_count = 0;
		uint32_t instance_count = 1;
		INDICES_TYPE indices_type = INDICES_TYPE_NONE;
		std::unordered_map<uint32_t, VertexAttributeInfo> bindings;
	public:
		void load(std::string path);
		void loadAsync(std::string path);
		virtual ~Mesh() = default;
		uint32_t getVertexCount() const;
		uint32_t getIndexCount() const;
		uint32_t getInstanceCount() const;
		size_t getIndicesSize() const;
		size_t getBindingSize(uint32_t binding) const;
		bool hasIndexBuffer() const;
		virtual void setVertexIndices(const uint32_t *vertices, size_t count) = 0;
		virtual void setVertexIndices(const uint16_t *vertices, size_t count) = 0;
		virtual void setBuffer(uint32_t location, const void *vertices, size_t count) = 0;
		virtual void setInstanceBuffer(uint32_t location, const void *data, size_t count) = 0;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

	};
}

