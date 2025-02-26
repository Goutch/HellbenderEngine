#pragma once

#include "Core.h"
#include <vector>
#include "glm/glm.hpp"
#include "Resource.h"
#include "unordered_map"
#include "string"

namespace HBE {
	class StorageBuffer;
	typedef uint32_t

	MESH_FLAGS;
	enum MESH_FLAG {
		MESH_FLAG_NONE = 0,
		MESH_FLAG_USED_IN_RAYTRACING = 1 << 0,
		MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER = 1 << 1,
	};

	enum MESH_TOPOLOGY {
		MESH_TOPOLOGY_TRIANGLE_LIST,
		MESH_TOPOLOGY_TRIANGLE_STRIP,
		MESH_TOPOLOGY_TRIANGLE_FAN,
		MESH_TOPOLOGY_LINE_LIST,
		MESH_TOPOLOGY_LINE_STRIP,
		MESH_TOPOLOGY_POINT_LIST,
	};

	struct MeshInfo {
		const VertexAttributeInfo *attribute_infos = nullptr;
		size_t attribute_info_count = 0;
		MESH_FLAGS flags = MESH_FLAG_NONE;
		MESH_TOPOLOGY topology = MESH_TOPOLOGY_TRIANGLE_LIST;
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
		std::unordered_map<uint32_t, VertexAttributeInfo> attributes_locations;
	public:
		void load(std::string path);

		void loadAsync(std::string path);

		virtual ~Mesh() = default;

		uint32_t getVertexCount() const;

		uint32_t getIndexCount() const;

		uint32_t getInstanceCount() const;

		size_t getIndicesSize() const;

		size_t getAttributeElementSize(uint32_t location) const;

		bool hasIndexBuffer() const;

		virtual void setVertexIndices(const uint32_t *vertices, size_t count) = 0;

		virtual void setVertexIndices(const uint16_t *vertices, size_t count) = 0;

		virtual void setBuffer(uint32_t location, const void *vertices, size_t count) = 0;

		virtual void setInstanceBuffer(uint32_t location, const void *data, size_t count) = 0;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual StorageBuffer *getAttributeStorageBuffer(uint32_t location) const = 0;

		virtual StorageBuffer *getIndicesStorageBuffer() const = 0;
	};
}

