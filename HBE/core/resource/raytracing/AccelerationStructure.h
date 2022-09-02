#pragma once

#include "Core.h"
#include "../Resource.h"

namespace HBE {
	class Mesh;

	struct AABBAccelerationStructureInfo {
		vec3 min;
		vec3 max;
	};

	class HB_API AABBAccelerationStructure : public Resource {

	};

	struct MeshAccelerationStructureInfo {
		Mesh *mesh;
	};

	class HB_API MeshAccelerationStructure : public Resource {

	};

	enum ACCELERATION_STRUCTURE_TYPE {
		ACCELERATION_STRUCTURE_TYPE_NONE = 0,
		ACCELERATION_STRUCTURE_TYPE_AABB = 1,
		ACCELERATION_STRUCTURE_TYPE_MESH = 2,
	};
	struct AccelerationStructureInstance {
		uint32_t acceleration_structure_index;
		mat4 transform;
		ACCELERATION_STRUCTURE_TYPE type;
	};
	struct RootAccelerationStructureInfo {
		AABBAccelerationStructure **aabb_acceleration_structures;
		MeshAccelerationStructure **mesh_acceleration_structures;
		uint32_t aabb_acceleration_structure_count;
		uint32_t mesh_acceleration_structure_count;
		AccelerationStructureInstance *instances;
		uint32_t instance_count;
	};

	class HB_API RootAccelerationStructure : public Resource {

	};
}

