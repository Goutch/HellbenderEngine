#pragma once

#include "Core.h"
#include "../Resource.h"
#include "core/graphics/Allocator.h"

namespace HBE
{
	class Mesh;

	struct AABBAccelerationStructureInfo
	{
		vec3 min;
		vec3 max;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
	};

	class HB_API AABBAccelerationStructure : public Resource
	{
	};

	struct MeshAccelerationStructureInfo
	{
		Mesh* mesh;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
	};

	class HB_API MeshAccelerationStructure : public Resource
	{
	};

	enum ACCELERATION_STRUCTURE_TYPE
	{
		ACCELERATION_STRUCTURE_TYPE_NONE = 0,
		ACCELERATION_STRUCTURE_TYPE_AABB = 1,
		ACCELERATION_STRUCTURE_TYPE_MESH = 2,
	};

	struct AccelerationStructureInstance
	{
		uint32_t acceleration_structure_index;
		uint32_t shader_group_index;
		mat4 transform;
		ACCELERATION_STRUCTURE_TYPE type;
		uint32_t custom_index = 0;
	};

	struct RootAccelerationStructureInfo
	{
		AABBAccelerationStructure** aabb_acceleration_structures;
		MeshAccelerationStructure** mesh_acceleration_structures;
		uint32_t aabb_acceleration_structure_count;
		uint32_t mesh_acceleration_structure_count;
		AccelerationStructureInstance* instances;
		uint32_t instance_count;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
	};

	class HB_API RootAccelerationStructure : public Resource
	{
	};
}
