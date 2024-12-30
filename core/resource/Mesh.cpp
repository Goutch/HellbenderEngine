#include "Mesh.h"
#include "core/resource/ResourceFactory.h"

namespace HBE {
	uint32_t Mesh::getVertexCount() const {
		return vertex_count;
	}

	uint32_t Mesh::getIndexCount() const {
		return index_count;
	}

	uint32_t Mesh::getInstanceCount() const {
		return instance_count;
	}

	bool Mesh::hasIndexBuffer() const {
		return indices_type != INDICES_TYPE_NONE;
	}

	size_t Mesh::getIndicesSize() const {
		return indices_type == INDICES_TYPE_UINT32 ? sizeof(uint32_t) : sizeof(uint16_t);
	}

	size_t Mesh::getAttributeElementSize(uint32_t location) const {
		return attributes_locations.at(location).size;
	}

}
