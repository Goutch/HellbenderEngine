#pragma once

#include "Core.h"
#include "../Resource.h"

namespace HBE {
	class Mesh;

	struct AABBAccelerationStructureInfo {
		vec3 max;
		vec3 min;
	};

	class HB_API AABBAccelerationStructure : public Resource {

	};

	struct MeshAccelerationStructureInfo {
		Mesh *mesh;
	};

	class HB_API MeshAccelerationStructure : public Resource {

	};

	struct RootAccelerationStructureInfo {

	};

	class HB_API RootAccelerationStructure : public Resource {

	};
}

